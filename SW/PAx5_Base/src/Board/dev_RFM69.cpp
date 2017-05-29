/**
This file is part of PAx5 (https://github.com/CalinRadoni/PAx5)
Copyright (C) 2016, 2017 by Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dev_RFM69.h"
#include "dev_RFM69_Regs.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

DEV_RFM69 sRadio;

// -----------------------------------------------------------------------------

// If there is not a matched antenna, better NOT Enable high power mode (+20dBm)
// because "The Duty Cycle of transmission at +20dBm is limited to 1%, with a
// maximum VSWR of 3:1 at antenna port, over the standard operating range"
// according to RFM69HW-V1.3 page 22

// -----------------------------------------------------------------------------

#define TIMEOUT_RFM69_INIT       ((uint32_t)100) // 100 ms
#define TIMEOUT_RFM69_ModeChange ((uint32_t)100) // 100 ms
#define TIMEOUT_RFM69_ReadRSSI   ((uint32_t)10)  // 10 ms (actually 1 ms should be enough)

// -----------------------------------------------------------------------------

// FXOSC = 32 MHz
// FSTEP = 32 MHZ / 2^19 = 61,035 Hz

#define RFM69_Mode_Sleep    0
#define RFM69_Mode_StandBy (1 << 2)
#define RFM69_Mode_FS      (2 << 2)
#define RFM69_Mode_TX      (3 << 2)
#define RFM69_Mode_RX      (4 << 2)

#define RFM69_DIO0_RX_PayloadReady 0x40
#define RFM69_DIO0_TX_PacketSent   0x00
#define RFM69_DIO0_Mask            0x3F

#define RFM69_DEFAULT_POWER 0x10
#define RFM69_DEFAULT_FREQ  ((uint32_t)867000000) // Frequency, according to ECC Rec. 70-03, SRD860, is 863 - 870 MHz

/** @brief RSSI threshold to consider a channel beeing free
 *
 * A good value should -90 dBm to -80 dBm.
 *
 * @attention Use positive value here, ex.: 85 for -85 dBm
 */
#define FreeAir_RSSI 85

#define ADDRESS_DEFAULT 0
#define ADDRESS_BROADCAST 0xFF

// -----------------------------------------------------------------------------

/** \brief Sync words
 *
 * A good sync word:
 * 	as many 1 to 0 transitions as possible
 * 	at most three identical consecutive bits
 * 	should start with a bit different from the last bit of the preamble (preamble: 1010...)
 * 	should be different from the preamble
 * 	should have good correlation (small or no side lobes)
 * 		convert sync word to a binary vector
 * 		run [r,lag] = xcorr(2 * binary_vector - 1) in http://octave-online.net
 * 		plot(r,lag) in http://octave-online.net to see the lobes
 */
//#define SYNC_WORD_TI		// TI CC1021 datasheet
#define SYNC_WORD_SILABS	// Silabs's EzRadio
//#define SYNC_WORD_CUSTOM

#define SYNC_WORD_LEN 2
#if SYNC_WORD_LEN == 2
	#ifdef SYNC_WORD_TI
		#define SYNC0 0xD3
		#define SYNC1 0x91
	#endif
	#ifdef SYNC_WORD_SILABS
		#define SYNCW0 0x2D
		#define SYNCW1 0xD4
	#endif
#elif SYNC_WORD_LEN == 4
	#ifdef SYNC_WORD_TI
		#define SYNC0 0xD3
		#define SYNC1 0x91
		#define SYNC2 0xDA
		#define SYNC3 0x26
	#endif
	#ifdef SYNC_WORD_SILABS
		#define SYNCW0 0x5A
		#define SYNCW1 0x0F
		#define SYNCW2 0xBE
		#define SYNCW3 0x66
	#endif
#else
	#error "Unknown Sync Word length !"
#endif

// -----------------------------------------------------------------------------

// default: bitrate 4800 b/s, fdev = 5 kHz, BW 7.8 kHz recommended 10.4 kHz
// DCC Cutoff Frequency 4%
// Modulation index = 2 * frequency deviation  / bitrate
// 0.5 <= 2 * Fdev/BR <= 10
// RxBw >= Fdev + BR/2
// RxBwAfc >=� Fdev + BR/2 + LOoffset (based of XTAL ppm)
// Fdev + BR/2 < 500kHz
// Offset = LowBetaAfcOffset * 488 Hz
// Offset MUST be > DC canceller�s cutoff frequency, set through DccFreqAfc in RegAfcBw

struct WorkConfig {
	uint8_t RegBitrateMsb;
	uint8_t RegBitrateLsb;
	uint8_t RegFdevMsb;
	uint8_t RegFdevLsb;
	uint8_t RegRxBw;
	uint8_t RegRxAfc;
	uint8_t RegTestAfc;
	uint8_t RegPacketConfig2;
};

#define COUNT_WorkConfig 9
const WorkConfig WorkConfigurations[COUNT_WorkConfig] = {
/* 0 */ {0x19, 0x00, 0x00, 0x52, 0x55, 0x6C, 0x02, 0x02}, // Bitrate   5k Fdev   5k Mod.Index 2   :: RX [Bandwidth  10k DccFreq 0,41k (3,98%)] :: AFC [BW  25k DccFreq 0,50k (1,99%) Offset  0,98k] :: InterPacketRxDelay 200us
/* 1 */ {0x06, 0x40, 0x01, 0x48, 0x44, 0x4B, 0x05, 0x02}, // Bitrate  20k Fdev  20k Mod.Index 2   :: RX [Bandwidth  31k DccFreq 1,24k (3,98%)] :: AFC [BW  50k DccFreq 1,99k (3,98%) Offset  2,44k] :: InterPacketRxDelay 50us
/* 2 */ {0x05, 0x00, 0x01, 0x9A, 0x53, 0x43, 0x06, 0x02}, // Bitrate  25k Fdev  25k Mod.Index 2   :: RX [Bandwidth  42k DccFreq 1,66k (3,98%)] :: AFC [BW  63k DccFreq 2,49k (3,98%) Offset  2,93k] :: InterPacketRxDelay 40us
/* 3 */ {0x02, 0x80, 0x03, 0x33, 0x52, 0x4A, 0x09, 0x12}, // Bitrate  50k Fdev  50k Mod.Index 2   :: RX [Bandwidth  83k DccFreq 3,32k (3,98%)] :: AFC [BW 100k DccFreq 3,98k (3,98%) Offset  4,39k] :: InterPacketRxDelay 40us
/* 4 */ {0x01, 0x40, 0x03, 0x33, 0x42, 0x42, 0x0B, 0x22}, // Bitrate 100k Fdev  50k Mod.Index 1   :: RX [Bandwidth 125k DccFreq 4,97k (3,98%)] :: AFC [BW 125k DccFreq 4,97k (3,98%) Offset  5,37k] :: InterPacketRxDelay 40us
/* 5 */ {0x01, 0x40, 0x06, 0x66, 0x51, 0x49, 0x11, 0x22}, // Bitrate 100k Fdev 100k Mod.Index 2   :: RX [Bandwidth 167k DccFreq 6,63k (3,98%)] :: AFC [BW 200k DccFreq 7,96k (3,98%) Offset  8,30k] :: InterPacketRxDelay 40us
/* 6 */ {0x01, 0x00, 0x04, 0x00, 0x51, 0x51, 0x0E, 0x32}, // Bitrate 125k Fdev  63k Mod.Index 1   :: RX [Bandwidth 167k DccFreq 6,63k (3,98%)] :: AFC [BW 167k DccFreq 6,63k (3,98%) Offset  6,83k] :: InterPacketRxDelay 64us
/* 7 */ {0x01, 0x00, 0x08, 0x00, 0x49, 0x41, 0x15, 0x32}, // Bitrate 125k Fdev 125k Mod.Index 2   :: RX [Bandwidth 200k DccFreq 7,96k (3,98%)] :: AFC [BW 250k DccFreq 9,95k (3,98%) Offset 10,25k] :: InterPacketRxDelay 64us
/* 8 */ {0x00, 0x80, 0x06, 0x66, 0x41, 0x41, 0x15, 0x42}  // Bitrate 250k Fdev 100k Mod.Index 0,8 :: RX [Bandwidth 250k DccFreq 9,95k (3,98%)] :: AFC [BW 250k DccFreq 9,95k (3,98%) Offset 10,25k] :: InterPacketRxDelay 64us
};

// -----------------------------------------------------------------------------

DEV_RFM69::DEV_RFM69() {
	configured = false;

	radioMode = RM_StandBy;

	initRM            = RMO_FSK;
	initDCF           = RDCF_None;
	initBR            = RBR_100kM1;
	initUseAFC        = true;
	initFilterAddress = true;
	initVariableLengthPacketFormat = true;
	nodeAddress       = ADDRESS_DEFAULT;

	changePacketLength = false;
	switchRX2RX        = true;
	switchTX2FS        = false;
	packetReceived     = false;
	packetReceivedLen  = 0;
	packetReceivedRSSI = -127;
	packetSent         = true;

	radioFreq          = RFM69_DEFAULT_FREQ;

	waitTimeForFreeAir = 10;

	interfaceError = false;
}

DEV_RFM69::~DEV_RFM69() {
}

// -----------------------------------------------------------------------------

uint8_t DEV_RFM69::ReadRegister(uint8_t reg)
{
	return sSPI.SendCmdPlusAndWait(reg & 0x7F, 0, SPISlave_Radio);
}

void DEV_RFM69::WriteRegister(uint8_t reg, uint8_t data)
{
	sSPI.SendCmdPlusAndWait(reg | 0x80, data, SPISlave_Radio);
}

// -----------------------------------------------------------------------------

uint8_t DEV_RFM69::GetModuleVersion(void)
{
	return ReadRegister(RFM69_RegVersion);
}

uint8_t DEV_RFM69::TestSyncRegister(uint8_t syncRegNo, uint8_t val)
{
	uint8_t reg = RFM69_RegSyncValue1 + (syncRegNo & 0x03);
	WriteRegister(reg, val);
	return ReadRegister(reg);
}

// -----------------------------------------------------------------------------

void DEV_RFM69::Initialize(void)
{
	uint8_t val;

	timeStart = sysTickCnt;
	interfaceError = false;

	// check module version ([7:4]=rev.number, [3:0]=metal mask rev.no.)
	do{
		if((sysTickCnt - timeStart) >= TIMEOUT_RFM69_INIT){ interfaceError = true; return; }
		val = ReadRegister(RFM69_RegVersion);
	}
	while(val < 0x24);

	// check if first sync register is writable
	do{
		if((sysTickCnt - timeStart) >= TIMEOUT_RFM69_INIT){ interfaceError = true; return; }
		WriteRegister(RFM69_RegSyncValue1, SYNCW0);
		val = ReadRegister(RFM69_RegSyncValue1);
	}
	while(val != SYNCW0);

	SetMode(RM_StandBy);

	switch(initRM){
		case RMO_FSK:  WriteRegister(RFM69_RegDataModul, 0x00); break;
		case RMO_GFSK: WriteRegister(RFM69_RegDataModul, 0x01); break;
		case RMO_OOK:  WriteRegister(RFM69_RegDataModul, 0x08); break;
		default:
			interfaceError = true;
			break;
	}
	if(interfaceError) return;

	SetFrequency(radioFreq);

	val = (uint8_t)initBR;
	if(val >= COUNT_WorkConfig) val = 0;
	WriteRegister(RFM69_RegBitrateMsb,    WorkConfigurations[val].RegBitrateMsb);
	WriteRegister(RFM69_RegBitrateLsb,    WorkConfigurations[val].RegBitrateLsb);
	WriteRegister(RFM69_RegFdevMsb,       WorkConfigurations[val].RegFdevMsb);
	WriteRegister(RFM69_RegFdevLsb,       WorkConfigurations[val].RegFdevLsb);
	WriteRegister(RFM69_RegRxBw,          WorkConfigurations[val].RegRxBw);
	WriteRegister(RFM69_RegAfcBw,         WorkConfigurations[val].RegRxAfc);
	WriteRegister(RFM69_RegTestAfc,       WorkConfigurations[val].RegTestAfc);
	WriteRegister(RFM69_RegPacketConfig2, WorkConfigurations[val].RegPacketConfig2);

	if(initUseAFC){
		WriteRegister(RFM69_RegAfcCtrl,  0x40); // Improved AFC routine (AfcLowBetaOn = 1)
		WriteRegister(RFM69_RegAfcFei,   0x1C); // AfcAutoclearOn, AfcAutoOn
		WriteRegister(RFM69_RegTestDagc, 0x20); // recommended value
	}
	else {
		WriteRegister(RFM69_RegAfcCtrl,  0x00); // Standard AFC routine
		WriteRegister(RFM69_RegAfcFei,   0x10);
		WriteRegister(RFM69_RegTestDagc, 0x30); // recommended value
	}

	val = 0x10; // CRC calculation/check (Tx/Rx) enabled
	if(initVariableLengthPacketFormat) val |= 0x80;
	switch(initDCF){
		case RDCF_None:      val |= 0x00; break; // just for completion
		case RDCF_Whitening: val |= 0x40; break;
		case RDCF_Mancester: val |= 0x20; break;
		default:
			interfaceError = true;
			break;
	}
	if(interfaceError) return;
	if(initFilterAddress){
		val |= 0x04;	// Address field must match NodeAddress or BroadcastAddress
		WriteRegister(RFM69_RegNodeAdrs, nodeAddress);
		WriteRegister(RFM69_RegBroadcastAdrs, ADDRESS_BROADCAST);
	}
	WriteRegister(RFM69_RegPacketConfig1, val);

	WriteRegister(RFM69_RegLna,           0x08); // default setting, 50 ohms, LNA gain set by internal AGC loop

	WriteRegister(RFM69_RegOcp,           0x1A); // default setting, OCP enabled @ 95mA
	WriteRegister(RFM69_RegDioMapping1,   0x00); // default value
	WriteRegister(RFM69_RegDioMapping2,   0x07); // No CLKOUT
	WriteRegister(RFM69_RegIrqFlags2,     0x10); // clear flag(s) and FIFO

	WriteRegister(RFM69_RegRssiThresh,    0xE4); // recommended 0xE4 = 228, sensitivity = -RFM69_RegRssiThresh/2 = - 114 dBm
	//WriteRegister(RFM69_RegRssiThresh,    0xC8); // sensitivity = -RFM69_RegRssiThresh/2 = - 100 dBm

#if SYNC_WORD_LEN == 2
	WriteRegister(RFM69_RegSyncConfig,    0x88); // 2 sync bytes
	WriteRegister(RFM69_RegSyncValue1,    SYNCW0);
	WriteRegister(RFM69_RegSyncValue2,    SYNCW1);
#elif SYNC_WORD_LEN == 4
	WriteRegister(RFM69_RegSyncConfig,    0x98); // 4 sync bytes
	WriteRegister(RFM69_RegSyncValue1,    SYNCW0);
	WriteRegister(RFM69_RegSyncValue2,    SYNCW1);
	WriteRegister(RFM69_RegSyncValue3,    SYNCW2);
	WriteRegister(RFM69_RegSyncValue4,    SYNCW3);
#endif

	WriteRegister(RFM69_RegPayloadLength, RFM69_FIFO_Size); // for packet mode: max length in Rx, not used in Tx
	WriteRegister(RFM69_RegFifoThresh,    0x8F); // TxStartCondition = FifoNotEmpty
	WriteRegister(RFM69_RegTestPa1,       0x55); // default, make sure NO +20dBm
	WriteRegister(RFM69_RegTestPa2,       0x70); // default, make sure NO +20dBm
	SetPowerOutput(RFM69_DEFAULT_POWER);
	SetPowerAmplifiers();

	changePacketLength = initVariableLengthPacketFormat;

	ConfigureInterrupt();

	configured = true;
}

void DEV_RFM69::SetNodeAddress(uint8_t addrIn)
{
	uint8_t val;

	nodeAddress = addrIn;

	val = 0x10; // CRC calculation/check (Tx/Rx) enabled
	if(initVariableLengthPacketFormat) val |= 0x80;
	switch(initDCF){
		case RDCF_None:      val |= 0x00; break; // just for completion
		case RDCF_Whitening: val |= 0x40; break;
		case RDCF_Mancester: val |= 0x20; break;
		default:
			interfaceError = true;
			break;
	}
	if(interfaceError) return;
	if(initFilterAddress){
		val |= 0x04;	// Address field must match NodeAddress or BroadcastAddress
		WriteRegister(RFM69_RegNodeAdrs, nodeAddress);
		WriteRegister(RFM69_RegBroadcastAdrs, ADDRESS_BROADCAST);
	}
	WriteRegister(RFM69_RegPacketConfig1, val);
}

void DEV_RFM69::SetMode(RadioMode modeIn)
{
	uint8_t val;

	radioMode = modeIn;
	switch(modeIn){
	case RM_Sleep:
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_Sleep);
		break;
	case RM_StandBy:
		SetPowerAmplifiers();
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_StandBy);
		break;
	case RM_FS:
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_FS);
		break;
	case RM_TX:
		// set power amplifiers
		SetPowerAmplifiers();
		// set interrupt
		val = ReadRegister(RFM69_RegDioMapping1);
		val = (val & RFM69_DIO0_Mask) | RFM69_DIO0_TX_PacketSent;
		WriteRegister(RFM69_RegDioMapping1, val);
		// change mode
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_TX);
		break;
	case RM_RX:
		// clear Flag(s) and FIFO
		WriteRegister(RFM69_RegIrqFlags2, 0x10);
		// set interrupt
		val = ReadRegister(RFM69_RegDioMapping1);
		val = (val & RFM69_DIO0_Mask) | RFM69_DIO0_RX_PayloadReady;
		WriteRegister(RFM69_RegDioMapping1, val);
		// set power amplifiers
		SetPowerAmplifiers();
		// change mode
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_RX);
		break;
	default:
		WriteRegister(RFM69_RegOpMode, RFM69_Mode_StandBy);
		radioMode = RM_StandBy;
		break;
	}

	// wait for mode to change
	timeStart = sysTickCnt;
	do{
		if((sysTickCnt - timeStart) >= TIMEOUT_RFM69_ModeChange){
			interfaceError = true;
			return;
		}
		val = ReadRegister(RFM69_RegIrqFlags1);
	}
	while((val & 0x80) == 0);
}

void DEV_RFM69::SetFrequency(uint32_t fIn)
{
	double dVal;
	uint8_t val;
	RadioMode currentRadioMode;

	// according to the RFM69HW-V1.3 datasheet, F = RegFrf[0:24] * 32MHz / 2^19

	dVal = fIn; dVal *= 256; dVal /= 15625;
	radioFreq = (uint32_t)dVal;

	currentRadioMode = radioMode;
	if(currentRadioMode == RM_TX) SetMode(RM_RX);

	val = (uint8_t)((radioFreq >> 16) & 0xFF);
	WriteRegister(RFM69_RegFrfMsb, val);
	val = (uint8_t)((radioFreq >> 8)  & 0xFF);
	WriteRegister(RFM69_RegFrfMid, val);
	val = (uint8_t)( radioFreq        & 0xFF);
	WriteRegister(RFM69_RegFrfLsb, val);

	val = ReadRegister(RFM69_RegFrfMsb);
	radioFreq = val; radioFreq = radioFreq << 8;
	val = ReadRegister(RFM69_RegFrfMid);
	radioFreq += val; radioFreq = radioFreq << 8;
	val = ReadRegister(RFM69_RegFrfLsb);
	radioFreq += val;

	if(currentRadioMode == RM_TX) SetMode(RM_TX);
	else if(currentRadioMode == RM_RX){
		SetMode(RM_FS);
		SetMode(RM_RX);
	}

	dVal = radioFreq; dVal *= 15625; dVal /= 256;
	radioFreq = (uint32_t)dVal;
}

uint32_t DEV_RFM69::GetFrequency(void)
{
	return radioFreq;
}

void DEV_RFM69::SetPowerAmplifiers(void)
{
	uint8_t val;

	// CHECK Radio: do I have to turn PA1 and PA2 off during reception or just RFM69_RegTestPa* ?
	if(radioMode == RM_TX){
		WriteRegister(RFM69_RegOcp, 0x0F); // disable OCP, 0x0F needed in case of high power
		val = ReadRegister(RFM69_RegPaLevel) & 0x1F;
		val |= paSelection;
		WriteRegister(RFM69_RegPaLevel, val);

		if(outputPower > 17){
			WriteRegister(RFM69_RegTestPa1, 0x5D);
			WriteRegister(RFM69_RegTestPa2, 0x7C);
		}
	}
	else{
		val = ReadRegister(RFM69_RegPaLevel) & 0x1F;
		val |= 0x80; // PA0
		WriteRegister(RFM69_RegPaLevel, val);
		WriteRegister(RFM69_RegOcp, 0x1A); // enable OCP

		if(outputPower > 17){
			WriteRegister(RFM69_RegTestPa1, 0x55);
			WriteRegister(RFM69_RegTestPa2, 0x70);
		}
	}
}

/** \brief Power output
 *
 *  Power output formulas [RFM69HW-V1.3 page 21]:
 * 		                PA0 On: -18dBm + powerLevel, range is -18 to +13 dBm
 * 		                PA1 On: -18dBm + powerLevel, range is  -2 to +13 dBm
 * 		          PA1 + PA2 On: -14dBm + powerLevel, range is  +2 to +17 dBm
 * 		PA1, PA2 and HighPower: -11dBm + powerLevel, range is  +5 to +20 dBm
 * where powerLevel = RFM69_RegPaLevel & 0x1F
 */
void DEV_RFM69::SetPowerOutput(int8_t pwrIn)
{
	uint8_t val;
	int8_t pwr;

	outputPower = pwrIn;
	if(outputPower < -18) outputPower = -18;
	if(outputPower >  19) outputPower =  19;

	if(outputPower < 14){
		// use PA0 for -18 to +13 dBm
		pwr = outputPower + 18;
		paSelection = 0x80;	// PA0 on
	}
	else{
		if(outputPower < 18){
			// use PA1 and PA2 for +14 to +17 dBm
			pwr = outputPower + 14;
			paSelection = 0x60; // PA1 on, PA2 on
		}
		else{
			// use PA1, PA2 and HighPower for +18 to +20 dBm
			pwr = outputPower + 11;
			paSelection = 0x60; // PA1 on, PA2 on
		}
	}
	val = paSelection + (uint8_t)pwr;
	WriteRegister(RFM69_RegPaLevel, val);
}

// -----------------------------------------------------------------------------

bool DEV_RFM69::IsFIFOEmpty(void)
{
	uint8_t val;

	val = ReadRegister(RFM69_RegIrqFlags2);
	return (val & 0x40) ? false : true;
}

void DEV_RFM69::ClearFIFO(void)
{
	WriteRegister(RFM69_RegIrqFlags2, 0x10);
}

void DEV_RFM69::ReadFIFO(void)
{
	uint8_t val;

	val = ReadRegister(RFM69_RegFifo);
	if(val > RFM69_MaxMsgLen) val = RFM69_MaxMsgLen;
	packetReceivedLen = val;

	radioBuffer[1] = RFM69_RegFifo & 0x7F;
	sSPI.SendBufferAndWait(&radioBuffer[1], packetReceivedLen + 1, SPISlave_Radio);

	if(changePacketLength)
		radioBuffer[RFM69_RadioBufferOffset]++;
}

bool DEV_RFM69::SendMessage(uint8_t lenIn)
{
	packetReceived = false;

	if(lenIn > RFM69_MaxMsgLen) return false;

	if(radioMode == RM_TX)
		SetMode(switchTX2FS ? RM_FS : RM_StandBy);

	WriteRegister(RFM69_RegIrqFlags2, 0x10); // Clears Flag(s) and FIFO

	if(changePacketLength)
		radioBuffer[RFM69_RadioBufferOffset]--;

	radioBuffer[0] = RFM69_RegFifo | 0x80;
	radioBuffer[1] = lenIn;
	sSPI.SendBufferAndWait(radioBuffer, lenIn + RFM69_RadioBufferOffset, SPISlave_Radio);

	packetSent = false;
	SetMode(RM_TX);
	return true;
}

bool DEV_RFM69::SendMessage(uint8_t *dataIn, uint8_t lenIn)
{
	if(dataIn == NULL) return true;

	if(lenIn > RFM69_MaxMsgLen) return false;

	if(waitTimeForFreeAir != 0){
		if(!WaitForFreeAir())
			return false;
	}
	else{
		if(radioMode == RM_TX)
			SetMode(switchTX2FS ? RM_FS : RM_StandBy);
	}

	WriteRegister(RFM69_RegIrqFlags2, 0x10); // Clears Flag(s) and FIFO

	if(changePacketLength)
		dataIn[RFM69_RadioBufferOffset]--;

	dataIn[0] = RFM69_RegFifo | 0x80;
	dataIn[1] = lenIn;
	sSPI.SendBufferAndWait(dataIn, lenIn + RFM69_RadioBufferOffset, SPISlave_Radio);

	packetSent = false;
	SetMode(RM_TX);
	return true;
}

bool DEV_RFM69::WaitForFreeAir(void)
{
	uint8_t val;
	bool done, res;

	if(radioMode != RM_RX)
		SetMode(RM_RX);

	res = false; done = false;
	timeStart = sysTickCnt;
	packetReceivedRSSI = -127;

	WriteRegister(RFM69_RegRssiConfig, 0x01); // request RSSI measurement
	do {
		val = ReadRegister(RFM69_RegRssiConfig);
		if((val & 0x02) != 0){
			// RSSI sampling is finished, read result
			val = ReadRegister(RFM69_RegRssiValue);

			// almost 'convert' result ...
			val = val >> 1;
			packetReceivedRSSI = 0 - val;
			// ... and compare
			if(val >= FreeAir_RSSI){
				// this channel is free
				done = true;
				res = true;
			}
			else res = false;

			if(!done)
				WriteRegister(RFM69_RegRssiConfig, 0x01); // request RSSI measurement
		}
		else{
			if((sysTickCnt - timeStart) >= waitTimeForFreeAir) {
				// no aquisition during wait time i.e. no signal detected (almost, see RFM69 manual)
				done = true;
				res = true;
			}
		}
	} while(!done);

	SetMode(RM_StandBy);
	return res;
}

void DEV_RFM69::ReadRSSI(void)
{
	uint8_t val;

	WriteRegister(RFM69_RegRssiConfig, 0x01); // request RSSI measurement
	timeStart = sysTickCnt;
	do{
		if((sysTickCnt - timeStart) >= TIMEOUT_RFM69_ReadRSSI){
			packetReceivedRSSI = 0;
			interfaceError = true;
			return;
		}
		val = ReadRegister(RFM69_RegRssiConfig);
	}
	while((val & 0x02) == 0);
	val = ReadRegister(RFM69_RegRssiValue); val = val >> 1; packetReceivedRSSI = 0 - val;
}

// -----------------------------------------------------------------------------

void DEV_RFM69::ConfigureInterrupt(void)
{
	// Enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	// PB is the source for EXTI1 interrupt
	SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~(SYSCFG_EXTICR1_EXTI1)) | SYSCFG_EXTICR1_EXTI1_PB;

	EXTI->IMR  = EXTI->IMR  | EXTI_IMR_IM1;     // enable Line 1
	EXTI->RTSR = EXTI->RTSR | EXTI_RTSR_TR1;    // enable rising edge
	EXTI->FTSR = EXTI->FTSR & ~(EXTI_FTSR_TR1); // disable falling edge

	NVIC_SetPriority(EXTI0_1_IRQn, NVIC_PRIORITY_EXTI_0_1);
	NVIC_EnableIRQ(EXTI0_1_IRQn);
}

// called from EXTI0_1_IRQHandler, defined in MainBoard.cpp
void DEV_RFM69::HandleInterrupt(void)
{
	uint8_t val, rssi;

	if(!configured) return;

	val = ReadRegister(RFM69_RegIrqFlags2);
	switch(radioMode){
	case RM_RX:
		if((val & 0x04) == 0x04){ // payload ready
			// Set in RX when the payload is ready. Cleared when FIFO is empty.
			rssi = ReadRegister(RFM69_RegRssiValue); rssi = rssi >> 1; packetReceivedRSSI = 0 - rssi;
			SetMode(RM_StandBy); // FIFO NOT cleared by this mode change
			ReadFIFO();
			packetReceived = true;

			// FIFO is cleared when changing from Sleep, Standby or TX to RX
			if(switchRX2RX)
				SetMode(RM_RX);
		}
		break;

	case RM_TX:
		if((val & 0x08) == 0x08){ // packet sent
			// Set in TX when the complete packet has been sent. Cleared when exiting TX.
			SetMode(switchTX2FS ? RM_FS : RM_StandBy); // FIFO cleared when changing from TX to any mode
			packetSent = true;
		}
		break;

	default:
		// should NOT get here
		//interfaceError = true;
		//NVIC_DisableIRQ(EXTI0_1_IRQn);
		break;
	}
}

// -----------------------------------------------------------------------------
}
