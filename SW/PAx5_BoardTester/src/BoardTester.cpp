/**
 * created 2016.07.25 by Calin Radoni
 */

#include "BoardTester.h"

#include "dev_LED.h"

#include "cpu_USART1.h"
#include "cpu_Info.h"
#include "cpu_SPI1.h"
#include "dev_ExternalFlash.h"
#include "dev_RFM69.h"
#include "cpu_Entropy.h"
#include "cpu_EntropyADC.h"
#include "cpu_ADC.h"
#include "cpu_I2C.h"
#include "cpu_Text.h"
#include "cpu_MemoryFlash.h"
#include "cpu_MemoryEEPROM.h"
#include "cpu_CRC.h"
#include "cpu_DMA.h"

#include "ext_HIHSensor.h"
#include "ext_WS2812.h"

#include "Enc_ChaCha20.h"

#include "PAW_CommProtocol.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

BoardTester::BoardTester() { }

// -----------------------------------------------------------------------------

void BoardTester::InteractiveTest(void)
{
	sUSART.ClearRXBuffer();

	sTextOutput.InitBuffer();

	sTextOutput.FormatAndOutputString("PAx5 Board Test, Firmware %d.%d.%d\r\n", Base_SW_VER_MAJOR, Base_SW_VER_MINOR, Base_SW_VER_BUILD);
	sTextOutput.FormatAndOutputString("Select option:\r\n");
	sTextOutput.FormatAndOutputString("\t0. Board\r\n");
	sTextOutput.FormatAndOutputString("\t1. Entropy\r\n");
	sTextOutput.FormatAndOutputString("\t2. Entropy ADC\r\n");
	sTextOutput.FormatAndOutputString("\t3. ADC\r\n");
	sTextOutput.FormatAndOutputString("\t4. I2C\r\n");
	sTextOutput.FormatAndOutputString("\t5. I2C - HIH Sensor\r\n");
	sTextOutput.FormatAndOutputString("\t6. I2C Slave\r\n");
	sTextOutput.FormatAndOutputString("\t7. WS2812 connected to PB5\r\n");
	sTextOutput.FormatAndOutputString("\t8. Encryption\r\n");
	sTextOutput.FormatAndOutputString("\t9. FLASH Programming !\r\n");
	sTextOutput.FormatAndOutputString("\tA. EEPROM Programming\r\n");
	sTextOutput.FormatAndOutputString("\tB. External FLASH Programming\r\n");
	sTextOutput.FormatAndOutputString("\tC. CRC32\r\n");
	sTextOutput.FormatAndOutputString("\tD. Comm.Protocol packets\r\n");
	sTextOutput.FormatAndOutputString("\tE. Copy: for loop vs DMA\r\n");
	sTextOutput.FormatAndOutputString("\tz. Show HW Log\r\n");
	sTextOutput.FormatAndOutputString("\tx. Clear HW Log\r\n");
	sTextOutput.FormatAndOutputString("\r\n");

	sTextOutput.Flush();

	while(!sUSART.dataReceived){
		// wait
	}
	switch(sUSART.bufferRX[0]){
		case '0':
			TestBoard();
			sRadio.Initialize();
			break;

		case '1': TestEntropy(); break;
		case '2': TestEntropyADC(); break;

		case '3': TestADC(); break;
		case '4': TestI2C(); break;
		case '5': TestI2C_HIH(); break;
		case '6': TestI2CSlave(); break;
		case '7': TestWS2812(); break;

		case '8': TestEcryption(); break;

		case '9': TestFLASHProg(); break;
		case 'A': TestEEPROMProg(); break;
		case 'B': TestExtFlash(); break;

		case 'C': TestCRC(); break;

		case 'D': TestPAWPackets(); break;

		case 'E': TestCopy(); break;

		case 'z': ShowHWLog(); break;
		case 'x': ClearHWLog(); break;
	}

	sTextOutput.FormatAndOutputString("\r\n\r\n");
	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::ShowHWLog(void)
{
	uint8_t i, cnt;

	sTextOutput.InitBuffer();

	for(i = 0, cnt = 0; i < MaxLogSize; i++){
		if(hwLogger.List[i].source != 0){
			cnt++;
			sTextOutput.FormatAndOutputString("Time %d Source %d Code %2x Data %4x\r\n",
					hwLogger.List[i].time,
					hwLogger.List[i].source,
					hwLogger.List[i].code,
					hwLogger.List[i].data);
		}
	}
	sTextOutput.FormatAndOutputString("HW Log: %d events.", cnt);

	sTextOutput.Flush();
}

void BoardTester::ClearHWLog(void)
{
	sTextOutput.InitBuffer();
	hwLogger.Initialize();
	sTextOutput.FormatAndOutputString("Log cleared.\r\n");
	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestBoard(void)
{
	uint8_t idx, clkDiv;
	uint8_t val;
	uint32_t dc;

	sTextOutput.InitBuffer();
	sTextOutput.FormatAndOutputString("PAx5 Board Test\r\n");
	sTextOutput.Flush();

	// CPU Info
	sCPU.ReadInfo();
	sTextOutput.FormatAndOutputString("CPU Info:\r\n");
#ifdef STM32L051xx
	sTextOutput.FormatAndOutputString("  - STM32L051xx");
#else
	sTextOutput.FormatAndOutputString("  - microcontroller unknown");
#endif
	sTextOutput.FormatAndOutputString("  - Device category %d, Rev.%c\r\n", sCPU.GetDevID(), sCPU.GetRevID());
	sTextOutput.FormatAndOutputString("  - Memory: %d kB\r\n", sCPU.GetFlashMemSize());
	sTextOutput.FormatAndOutputString("  - Unique ID: %8x %8x %8x\r\n", sCPU.uniqueID[0], sCPU.uniqueID[1], sCPU.uniqueID[2]);

	dc = sCPU.Get_CPUID_Implementer();
	sTextOutput.FormatAndOutputString("CPUID:\r\n");
	if(dc == 0x41) sTextOutput.FormatAndOutputString("  - Implementer ARM\r\n");
	else sTextOutput.FormatAndOutputString("  - Implementer unknown %x\r\n", dc);
	dc = sCPU.Get_CPUID_Architecture();
	if(dc == 0x0C) sTextOutput.FormatAndOutputString("  - Architecture ARMv6-M\r\n");
	else sTextOutput.FormatAndOutputString("  - Unknown architecture %x\r\n", dc);
	dc = sCPU.Get_CPUID_PartNo();
	if(dc == 0x0C60) sTextOutput.FormatAndOutputString("  - STM32L0 Cortex-M0+\r\n");
	else sTextOutput.FormatAndOutputString("  - Unknown part number %x\r\n", dc);
	dc = sCPU.Get_CPUID_Variant(); 	sTextOutput.FormatAndOutputString("  - Revision %d, ", dc);
	dc = sCPU.Get_CPUID_Revision(); sTextOutput.FormatAndOutputString("patch %d\r\n", dc);

	sTextOutput.FormatAndOutputString("\r\n");
	sTextOutput.Flush();

	// SPI: External FLASH and Radio

	clkDiv = sSPI.clockDivider;
	for(idx = 0; idx < 8; idx++){
		sSPI.clockDivider = idx;
		sSPI.Configure();

		sTextOutput.FormatAndOutputString("SPI clock divider: %d\r\n", sSPI.clockDivider);
		sTextOutput.Flush();

		sExternalFlash.ReadElectronicSignature();
		sTextOutput.FormatAndOutputString("  - Memory Signature: 0x%x, ", sExternalFlash.dataBuf[4]);
		sExternalFlash.ReadIdentification();
		sTextOutput.FormatAndOutputString("Memory ID: 0x%x 0x%x 0x%x\r\n",
				sExternalFlash.dataBuf[1], sExternalFlash.dataBuf[2], sExternalFlash.dataBuf[3]);
		sTextOutput.Flush();

		val = sRadio.GetModuleVersion();
		sTextOutput.FormatAndOutputString("  - RFM69 version: 0x%x\r\n", val);

		sTextOutput.FormatAndOutputString("  - RFM69 RegSync3 check: ");
		val = sRadio.TestSyncRegister(3, 0x55);
		sTextOutput.FormatAndOutputString("0x%x ", val);
		val = sRadio.TestSyncRegister(3, 0xAA);
		sTextOutput.FormatAndOutputString("0x%x\r\n", val);

		if(sSPI.clockDivider == 7)
			sTextOutput.FormatAndOutputString("\r\n");
		sTextOutput.Flush();
	}
	sSPI.clockDivider = clkDiv;
	sSPI.Configure();

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestEntropy(void)
{
	uint32_t eCnt;
	uint8_t i;

	sLED.On();

	sTextOutput.InitBuffer();

	i = 0;
	eCnt = 4 * EntropyBufferLen;
	while(eCnt > 0){
		if(i == 0) sTextOutput.FormatAndOutputString("\t");
		sTextOutput.FormatAndOutputString(" %2x", entropy.Get8bits());
		if(++i == 16){
			i = 0;
			sTextOutput.FormatAndOutputString("\r\n");
		}
		--eCnt;
	}

	i = 0;
	eCnt = EntropyBufferLen;
	while(eCnt > 0){
		if(i == 0) sTextOutput.FormatAndOutputString("\t");
		sTextOutput.FormatAndOutputString(" %8x", entropy.Get32bits());
		if(++i == 8){
			i = 0;
			sTextOutput.FormatAndOutputString("\r\n");
		}
		--eCnt;
	}
	sTextOutput.Flush();

	entropy.Disable();

	sLED.Off();
}

void BoardTester::TestEntropyADC(void)
{
	uint32_t eCnt;

	sLED.On();
	entropyADC.Enable();

	sTextOutput.InitBuffer();
	eCnt = 16;
	while(eCnt > 0){
		sTextOutput.FormatAndOutputString("\t");

		entropyADC.CollectStart();
		while(!entropyADC.EntropyCollected()) { __NOP(); } // wait
		for(uint8_t i = 0; i < EntropyBufferLenADC; i++)
			sTextOutput.FormatAndOutputString(" %2x", entropyADC.buffer[i]);

		entropyADC.CollectStart();
		while(!entropyADC.EntropyCollected()) { __NOP(); } // wait
		for(uint8_t i = 0; i < EntropyBufferLenADC; i++)
			sTextOutput.FormatAndOutputString(" %2x", entropyADC.buffer[i]);

		sTextOutput.FormatAndOutputString("\r\n");
		--eCnt;
	}
	sTextOutput.Flush();

	entropyADC.Disable();
	sLED.Off();
}

// -----------------------------------------------------------------------------

void BoardTester::TestADC(void)
{
	uint8_t round;

	//TODO ZZZZZZZ board.InitializeADCInput(0x07);

	round = 0;
	sTextOutput.InitBuffer();
	sADC.oversamplingRatio = 0;
	while(round < 3){
		if(sADC.oversamplingRatio == 0){
			sADC.GetC1(); sTextOutput.FormatAndOutputString("C1: %u ", sADC.data);
			sADC.GetC2(); sTextOutput.FormatAndOutputString("C2: %u ", sADC.data);
			sADC.GetC3(); sTextOutput.FormatAndOutputString("C3: %u\r\n", sADC.data);
		}

		sADC.ReadVDDA();             sTextOutput.FormatAndOutputString("VDDA: %u[V*100]\r\n", sADC.data);
		sADC.ReadChannel(17);        sTextOutput.FormatAndOutputString("Vref: %u = ", sADC.data);
		sADC.ConvertDataToVoltage(); sTextOutput.FormatAndOutputString("%u[V*100]\r\n", sADC.data);
		sADC.ReadChannel(18);
			sTextOutput.FormatAndOutputString("Vtemp: %u, ", sADC.data);
			sTextOutput.FormatAndOutputString("Temp: %i%cC\r\n", sADC.ConvertDataToTemperature(), (uint8_t)0xB0);

		sTextOutput.FormatAndOutputString("%d ", sADC.oversamplingRatio);
		sADC.ReadChannel(A0); sTextOutput.FormatAndOutputString("A0: %u ", sADC.data);
		sADC.ReadChannel(A1); sTextOutput.FormatAndOutputString("A1: %u ", sADC.data);
		sADC.ReadChannel(A2); sTextOutput.FormatAndOutputString("A2: %u ", sADC.data);
		sADC.ReadChannel(A3); sTextOutput.FormatAndOutputString("A3: %u\r\n", sADC.data);

		sADC.oversamplingRatio++;
		sADC.oversamplingRatio &= 0x07;
		if(sADC.oversamplingRatio == 0){
			sTextOutput.FormatAndOutputString("\r\n");
			round++;
		}

		sTextOutput.Flush();
		Delay(1000);
	}

	//TODO ZZZZZZZ board.InitializeADCInput(0x00);
}

// -----------------------------------------------------------------------------

void BoardTester::TestI2C(void)
{
	uint8_t addr;

	sI2C.Enable();
	sTextOutput.InitBuffer();
	for(addr = 0; addr < 128; addr++){
		sI2C.buffLen = 0;
		sI2C.Write(addr);
		switch(sI2C.status) {
			case I2CStatus_OK:
				sTextOutput.FormatAndOutputString("Found a device at address 0x%2x\r\n", addr);
				sTextOutput.Flush();
				break;
			case I2CStatus_NACK:
				break;
			case I2CStatus_IntfErr:
				sTextOutput.FormatAndOutputString("Error, exit.\r\n", addr);
				sTextOutput.Flush();
				addr = 0xFF;
				break;
		}
	}
	sI2C.Disable();
}

void BoardTester::TestI2C_HIH(void)
{
	uint32_t ts, te;
	bool dataOK;
	uint8_t round;
	EXT_HIHSensor hihSensor;
	EXT_HIHSensor::Status res;

	sI2C.Enable();
	sTextOutput.InitBuffer();
	round = 0;
	while(round < 10){
		res = hihSensor.ReadInit();
		ts = sysTickCnt;
		if(res != EXT_HIHSensor::Status::DATA_OK){
			sTextOutput.FormatAndOutputString("ReadInit failed (code %d) !\r\n", res);
			sTextOutput.Flush();
			round++;
			Delay(1000);
		}
		else{
			dataOK = false;
			while(!dataOK){
				res = hihSensor.ReadData();
				te = sysTickCnt - ts;
				if(res == EXT_HIHSensor::Status::DATA_OK){
					sTextOutput.FormatAndOutputString("Humidity %d %d[%%*10], Temperature %d %d[%cC*10], %d ms\r\n",
							hihSensor.rawH, hihSensor.GetHumidity(), hihSensor.rawT, hihSensor.GetTemperature() - 2731, 0xB0, te);
					sTextOutput.Flush();
					dataOK = true;
					round++;
				}
				else{
					if(res == EXT_HIHSensor::Status::DATA_Stale) Delay(1);
					else{
						// error
						sTextOutput.FormatAndOutputString("Error %d !\r\n", res);
						sTextOutput.Flush();
						dataOK = true;
						round++;
					}
				}
			}
		}
	}
	sI2C.Disable();
}

volatile uint8_t TestI2CSlaveDataIn, TestI2CSlaveDataLen, TestI2CSlaveDataRX, TestI2CSlaveDataTX;
void TestI2CSlaveReceive(uint8_t rxLen)
{
	TestI2CSlaveDataLen = rxLen;
	TestI2CSlaveDataIn = sI2C.buffer[0];

	TestI2CSlaveDataRX++;
}
void TestI2CSlaveRequest(void)
{
	uint8_t i;

	for(i = 0; i < I2CBufferLen; i++)
		sI2C.buffer[i] = TestI2CSlaveDataIn + i + 1;
	sI2C.buffLen = I2CBufferLen;

	TestI2CSlaveDataTX++;
}
void BoardTester::TestI2CSlave(void)
{
	uint8_t addr, i;

	addr = 0x28;
	sI2C.EnableAsSlave(addr);
	sTextOutput.InitBuffer();
	sTextOutput.FormatAndOutputString("Initialized as slave with addr 0x%2x\r\n", addr);
	sTextOutput.FormatAndOutputString("Send 'x' to finish test\r\n", addr);
	sTextOutput.Flush();

	TestI2CSlaveDataIn = '0';
	TestI2CSlaveDataRX = 0;
	TestI2CSlaveDataTX = 0;
	sI2C.onReceive(TestI2CSlaveReceive);
	sI2C.onRequest(TestI2CSlaveRequest);

	while(TestI2CSlaveDataIn != 'x'){
		if(TestI2CSlaveDataRX != 0){
			sTextOutput.FormatAndOutputString("RX[%d]: ", TestI2CSlaveDataLen);
			for(i = 0; i < TestI2CSlaveDataLen; i++)
				sTextOutput.FormatAndOutputString("0x%2x ", sI2C.buffer[i]);
			sTextOutput.FormatAndOutputString("\r\n");
			TestI2CSlaveDataRX = 0;
			sTextOutput.Flush();
		}
		if(TestI2CSlaveDataTX != 0){
			sTextOutput.FormatAndOutputString("TX[ ]: ");
			for(i = 0; i < 3; i++)
				sTextOutput.FormatAndOutputString("0x%2x ", sI2C.buffer[i]);
			sTextOutput.FormatAndOutputString("...\r\n\r\n");
			TestI2CSlaveDataTX = 0;
			sTextOutput.Flush();
		}
	}
	sI2C.Disable();
}

// -----------------------------------------------------------------------------

void BoardTester::TestWS2812(void)
{
	sTextOutput.InitBuffer();
	ws2812.Initialize(EXT_WS2812::WSPinName::PB5);

	for(uint8_t round = 0; round < 10; round++){
		ws2812.InitData();
		ws2812.AddData(0x0F, 0x00, 0x00); ws2812.AddData(0x00, 0x0F, 0x00); ws2812.AddData(0x00, 0x00, 0x0F);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);

		ws2812.InitData();
		ws2812.AddData(0x0F, 0x0F, 0x00); ws2812.AddData(0x00, 0x0F, 0x0F); ws2812.AddData(0x0F, 0x00, 0x0F);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);

		ws2812.InitData();
		ws2812.AddData(0x00, 0x0F, 0x00); ws2812.AddData(0x00, 0x00, 0x0F); ws2812.AddData(0x0F, 0x00, 0x00);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);

		ws2812.InitData();
		ws2812.AddData(0x00, 0x0F, 0x0F); ws2812.AddData(0x0F, 0x00, 0x0F); ws2812.AddData(0x0F, 0x0F, 0x00);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);

		ws2812.InitData();
		ws2812.AddData(0x00, 0x00, 0x0F); ws2812.AddData(0x0F, 0x00, 0x00); ws2812.AddData(0x00, 0x0F, 0x00);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);

		ws2812.InitData();
		ws2812.AddData(0x0F, 0x00, 0x0F); ws2812.AddData(0x0F, 0x0F, 0x00); ws2812.AddData(0x00, 0x0F, 0x0F);
		ws2812.SendSPIBuffer(); board.CheckRadioInterrupt(); Delay(1000);
	}
}

// -----------------------------------------------------------------------------

void BoardTester::TestEcryption(void)
{
	PAx5CommProtocol::Enc_ChaCha20 chacha;
	PAx5CommProtocol::CryptoContext  encCtx;
	uint8_t i;
	uint8_t data[64];
	const uint8_t test1[64] = {0x10, 0xf1, 0xe7, 0xe4, 0xd1, 0x3b, 0x59, 0x15, 0x50, 0x0f, 0xdd, 0x1f, 0xa3, 0x20, 0x71, 0xc4, 0xc7, 0xd1, 0xf4, 0xc7, 0x33, 0xc0, 0x68, 0x03, 0x04, 0x22, 0xaa, 0x9a, 0xc3, 0xd4, 0x6c, 0x4e, 0xd2, 0x82, 0x64, 0x46, 0x07, 0x9f, 0xaa, 0x09, 0x14, 0xc2, 0xd7, 0x05, 0xd9, 0x8b, 0x02, 0xa2, 0xb5, 0x12, 0x9c, 0xd1, 0xde, 0x16, 0x4e, 0xb9, 0xcb, 0xd0, 0x83, 0xe8, 0xa2, 0x50, 0x3c, 0x4e};
	const uint8_t test2ks[114] = {0x22, 0x4f, 0x51, 0xf3, 0x40, 0x1b, 0xd9, 0xe1, 0x2f, 0xde, 0x27, 0x6f, 0xb8, 0x63, 0x1d, 0xed, 0x8c, 0x13, 0x1f, 0x82, 0x3d, 0x2c, 0x06, 0xe2, 0x7e, 0x4f, 0xca, 0xec, 0x9e, 0xf3, 0xcf, 0x78, 0x8a, 0x3b, 0x0a, 0xa3, 0x72, 0x60, 0x0a, 0x92, 0xb5, 0x79, 0x74, 0xcd, 0xed, 0x2b, 0x93, 0x34, 0x79, 0x4c, 0xba, 0x40, 0xc6, 0x3e, 0x34, 0xcd, 0xea, 0x21, 0x2c, 0x4c, 0xf0, 0x7d, 0x41, 0xb7, 0x69, 0xa6, 0x74, 0x9f, 0x3f, 0x63, 0x0f, 0x41, 0x22, 0xca, 0xfe, 0x28, 0xec, 0x4d, 0xc4, 0x7e, 0x26, 0xd4, 0x34, 0x6d, 0x70, 0xb9, 0x8c, 0x73, 0xf3, 0xe9, 0xc5, 0x3a, 0xc4, 0x0c, 0x59, 0x45, 0x39, 0x8b, 0x6e, 0xda, 0x1a, 0x83, 0x2c, 0x89, 0xc1, 0x67, 0xea, 0xcd, 0x90, 0x1d, 0x7e, 0x2b, 0xf3, 0x63};
	const uint8_t test2pt[114] = {0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20, 0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73, 0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69, 0x74, 0x2e};
	const uint8_t test2ct[114] = {0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80, 0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81, 0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2, 0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b, 0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab, 0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57, 0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab, 0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8, 0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61, 0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e, 0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06, 0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36, 0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6, 0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42, 0x87, 0x4d};
	bool ok;

	sTextOutput.InitBuffer();

	encCtx.use256bitsEncKey = true;

	// --- Test 1 ---------------------------------------
	encCtx.key[0] = 0x03020100; encCtx.key[1] = 0x07060504; encCtx.key[2] = 0x0b0a0908; encCtx.key[3] = 0x0f0e0d0c;
	encCtx.key[4] = 0x13121110; encCtx.key[5] = 0x17161514; encCtx.key[6] = 0x1b1a1918; encCtx.key[7] = 0x1f1e1d1c;
	encCtx.cnonce[0]  = 0x00000001;
	encCtx.cnonce[1] = 0x09000000; encCtx.cnonce[2] = 0x4a000000; encCtx.cnonce[3] = 0x00000000;


	for(i=0; i<64; i++) data[i] = 0;
	chacha.EncryptBlock(&encCtx, data, 64);
	ok = true;
	for(i=0; i<64; i++){
		if(data[i] != test1[i]){
			sTextOutput.FormatAndOutputString("Failed test 1 at index %d !\r\n", i);
			ok = false;
		}
	}
	if(ok) sTextOutput.FormatAndOutputString("Test 1 passed.\r\n");
	sTextOutput.Flush();

	// --- Test 2 key stream ---------------------------------------
	ok = true;
	encCtx.key[0] = 0x03020100; encCtx.key[1] = 0x07060504; encCtx.key[2] = 0x0b0a0908; encCtx.key[3] = 0x0f0e0d0c;
	encCtx.key[4] = 0x13121110; encCtx.key[5] = 0x17161514; encCtx.key[6] = 0x1b1a1918; encCtx.key[7] = 0x1f1e1d1c;
	encCtx.cnonce[1] = 0x00000000; encCtx.cnonce[2] = 0x4a000000; encCtx.cnonce[3] = 0x00000000;
	encCtx.cnonce[0] = 0x00000001;
	for(i=0; i<64; i++) data[i] = 0;
	chacha.EncryptBlock(&encCtx, data, 64);
	for(i=0; i<64; i++){
		if(data[i] != test2ks[i]){
			sTextOutput.FormatAndOutputString("Failed test 2 key stream at index %d !\r\n", i);
			ok = false;
		}
	}
	encCtx.cnonce[0] = 0x00000002;
	for(i=0; i<50; i++) data[i] = 0;
	chacha.EncryptBlock(&encCtx, data, 50);
	for(i=0; i<50; i++){
		if(data[i] != test2ks[64 + i]){
			sTextOutput.FormatAndOutputString("Failed test 2 key stream at index %d !\r\n", i + 64);
			ok = false;
		}
	}
	if(ok) sTextOutput.FormatAndOutputString("Test 2 key stream passed.\r\n");
	sTextOutput.Flush();

	// --- Test 2 encryption ---------------------------------------
	ok = true;
	encCtx.key[0] = 0x03020100; encCtx.key[1] = 0x07060504; encCtx.key[2] = 0x0b0a0908; encCtx.key[3] = 0x0f0e0d0c;
	encCtx.key[4] = 0x13121110; encCtx.key[5] = 0x17161514; encCtx.key[6] = 0x1b1a1918; encCtx.key[7] = 0x1f1e1d1c;
	encCtx.cnonce[1] = 0x00000000; encCtx.cnonce[2] = 0x4a000000; encCtx.cnonce[3] = 0x00000000;
	encCtx.cnonce[0] = 0x00000001;
	for(i=0; i<64; i++) data[i] = test2pt[i];
	chacha.EncryptBlock(&encCtx, data, 64);
	for(i=0; i<64; i++){
		if(data[i] != test2ct[i]){
			sTextOutput.FormatAndOutputString("Failed test 2 encryption at index %d !\r\n", i);
			ok = false;
		}
	}
	encCtx.cnonce[0] = 0x00000002;
	for(i=0; i<50; i++) data[i] = test2pt[i + 64];
	chacha.EncryptBlock(&encCtx, data, 50);
	for(i=0; i<50; i++){
		if(data[i] != test2ct[i + 64]){
			sTextOutput.FormatAndOutputString("Failed test 2 encryption at index %d !\r\n", i + 64);
			ok = false;
		}
	}
	if(ok) sTextOutput.FormatAndOutputString("Test 2 encryption passed.\r\n");
	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestCopy(void)
{
	uint32_t i, j, k, err;
	uint32_t ts;

	sTextOutput.InitBuffer();

	for(i = 0; i < TestBuffLen; i++)
		buffS[i] = (uint8_t)i;

	k = 10000;

	ts = sysTickCnt;
	for(j = 0; j < k; j++)
		for(i = 0; i < TestBuffLen; i++)
			buffD[i] = buffS[i];
	ts = sysTickCnt - ts;
	sTextOutput.FormatAndOutputString("for loop: %d ms\r\n", ts);

	for(i = 0; i < TestBuffLen; i++){
		buffD[i] = 0;
		if(i < 10)
			sTextOutput.FormatAndOutputString("%d-%d ", buffS[i], buffD[i]);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	ts = sysTickCnt;
	err = 0;
	sDMA.M2M_Initialize();
	for(j = 0; j < k; j++){
		sDMA.M2M_Copy((uint32_t)buffS, (uint32_t)buffD, TestBuffLen);
		while(sDMA.M2M_TransferStatus() == 0){
			__NOP();
		}
		if(sDMA.M2M_TransferStatus() != 1) err++;
	}
	sDMA.M2M_CleanUp();
	ts = sysTickCnt - ts;
	sTextOutput.FormatAndOutputString("DMA: %d ms, %d errors\r\n", ts, err);

	j = 0;
	for(i = 0; i < TestBuffLen; i++){
		if(buffD[i] != buffS[i]) j++;
		if(i < 10)
			sTextOutput.FormatAndOutputString("%d-%d ", buffS[i], buffD[i]);
	}
	sTextOutput.FormatAndOutputString("... %d errors\r\n", j);

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestFLASHProg(void)
{
	uint32_t startAddress, addr, dw;
	uint8_t i, j;
	bool res;
	uint8_t wdata[CPUMF_HALFPAGE_SIZE_Bytes];

	sTextOutput.InitBuffer();

	startAddress = 0x0800FF00;	// start of page 510

	CPU_MemoryFLASH_address = startAddress;
	sTextOutput.FormatAndOutputString("Using page %d, halfpage %d, address %x\r\n",
			CPU_MemoryFLASH_GetPage(), CPU_MemoryFLASH_GetHalfPage(), 0x0800FF00);

	entropy.Enable();
	entropy.CollectStart();
	while(!entropy.EntropyCollected()) { __NOP(); }
	for(i = 0, j = 0; i < CPUMF_HALFPAGE_SIZE_Bytes; i++){
		wdata[i] = entropy.buffer[j];
		j++;
		if(j == EntropyBufferLen){
			j = 0;
			entropy.CollectStart();
			while(!entropy.EntropyCollected()) { __NOP(); }
		}
	}
	entropy.Disable();

	res = CPU_MemoryFLASH_Unlock();
	if(!res){
		sTextOutput.FormatAndOutputString("Failed to unlock FLASH !\r\n");
		sTextOutput.Flush();
		return;
	}
	sTextOutput.FormatAndOutputString("FLASH Unlocked.\r\n");

	sTextOutput.FormatAndOutputString("Original data:");
	for(i = 0, addr = startAddress; i < CPUMF_HALFPAGE_SIZE_DW; i++, addr += 4){
		dw = *(__IO uint32_t *)addr;
		sTextOutput.FormatAndOutputString(" %8x", dw);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	CPU_MemoryFLASH_address = startAddress;
	res = CPU_MemoryFLASH_ErasePage();
	if(!res){
		CPU_MemoryFLASH_Lock();
		sTextOutput.FormatAndOutputString("Failed to erase page %d !\r\n", CPU_MemoryFLASH_GetPage());
		sTextOutput.Flush();
		return;
	}
	sTextOutput.FormatAndOutputString("Page %d erased.\r\n", CPU_MemoryFLASH_GetPage());

	sTextOutput.FormatAndOutputString("Data:");
	for(i = 0, addr = startAddress; i < CPUMF_HALFPAGE_SIZE_DW; i++, addr += 4){
		dw = *(__IO uint32_t *)addr;
		sTextOutput.FormatAndOutputString(" %8x", dw);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	sTextOutput.FormatAndOutputString("Buff:");
	for(i = 0; i < CPUMF_HALFPAGE_SIZE_Bytes; i++){
		if((i & 0x03) == 0) sTextOutput.FormatAndOutputString(" ");
		sTextOutput.FormatAndOutputString("%2x", wdata[i]);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	CPU_MemoryFLASH_address = startAddress;
	res = CPU_MemoryFLASH_WriteHalfPage(wdata);
	if(!res){
		CPU_MemoryFLASH_Lock();
		sTextOutput.FormatAndOutputString("Failed to write half page %d !\r\n", CPU_MemoryFLASH_GetHalfPage());
		sTextOutput.Flush();
		return;
	}
	sTextOutput.FormatAndOutputString("Half page %d written.\r\n", CPU_MemoryFLASH_GetHalfPage());

	sTextOutput.FormatAndOutputString("Data:");
	for(i = 0, addr = startAddress; i < CPUMF_HALFPAGE_SIZE_DW; i++, addr += 4){
		dw = *(__IO uint32_t *)addr;
		sTextOutput.FormatAndOutputString(" %8x", dw);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	CPU_MemoryFLASH_Lock();
	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestEEPROMProg(void)
{
	uint8_t i, testLen;
	uint32_t buff[5];
	uint32_t workAddress;
	uint32_t rVal;
	bool res;

	testLen = sizeof(buff)/sizeof(uint32_t);
	workAddress = 0;

	sTextOutput.InitBuffer();

	entropy.Enable();
	entropy.CollectStart();

	sTextOutput.FormatAndOutputString("Data at address %d:", workAddress);
	for(i = 0; i < testLen; i++){
		rVal = sMemEEPROM.Read32(workAddress + (i<<2));
		sTextOutput.FormatAndOutputString(" %8x", rVal);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	if(!sMemEEPROM.Unlock()){
		sTextOutput.FormatAndOutputString("Failed to unlock EEPROM !\r\n");
		sTextOutput.Flush();
		return;
	}

	sTextOutput.FormatAndOutputString("Writting at address %d:", workAddress);
	for(i = 0; i < testLen; i++){
		buff[i] = entropy.Get32bits();
		if(!sMemEEPROM.Write32(workAddress + (i<<2), buff[i]))
			sTextOutput.FormatAndOutputString(" fail@%i", workAddress + (i<<2));
	}
	sTextOutput.FormatAndOutputString(" done\r\n");

	entropy.Disable();

	res = true;
	sTextOutput.FormatAndOutputString("Data at address %d:", workAddress);
	for(i = 0; i < testLen; i++){
		rVal = sMemEEPROM.Read32(workAddress + (i<<2));
		sTextOutput.FormatAndOutputString(" %8x", rVal);
		if(rVal != buff[i])
			res = false;
	}
	if(res) sTextOutput.FormatAndOutputString(" OK\r\n");
	else    sTextOutput.FormatAndOutputString(" ERROR !\r\n");

	sTextOutput.FormatAndOutputString("Zero:");
	for(i = 0; i < testLen; i++){
		if(!sMemEEPROM.Write32(workAddress + (i<<2), 0))
			sTextOutput.FormatAndOutputString(" fail@%i", workAddress + (i<<2));
	}
	sTextOutput.FormatAndOutputString(" done\r\n");

	sMemEEPROM.Lock();

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestExtFlash(void)
{
	uint16_t i;
	uint16_t testLength;
	uint32_t testAddress;

	testLength  = sExternalFlash.GetPageSize();
	testAddress = 0x0001FF00;

	sTextOutput.InitBuffer();

	if(!sExternalFlash.Initialize()){
		sTextOutput.FormatAndOutputString("Failed to initialize external FLASH !\r\n");
		sTextOutput.Flush();
		return;
	}
	sTextOutput.FormatAndOutputString("External FLASH Initialized.\r\n");

	sExternalFlash.SetAddress(testAddress);
	sTextOutput.FormatAndOutputString("Erase Sector %d\r\n", sExternalFlash.GetSector());
	sExternalFlash.SectorErase();

	sTextOutput.FormatAndOutputString("Check page %d:", sExternalFlash.GetPageNumber());
	sExternalFlash.dataLen = testLength;
	sExternalFlash.ReadData();
	for(i = 0; i < testLength; i++){
		if(sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i] != 0xFF)
			sTextOutput.FormatAndOutputString(" error:%d=%x", i, sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i]);
	}
	sTextOutput.FormatAndOutputString(" done.\r\n");

	sTextOutput.FormatAndOutputString("Writing 0x55 in page %d\r\n", sExternalFlash.GetPageNumber());
	for(i = 0; i < testLength; i++)
		sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i] = 0x55;
	sExternalFlash.dataLen = testLength;
	sExternalFlash.WriteData();

	sTextOutput.FormatAndOutputString("Check if page %d contains 0x55:", sExternalFlash.GetPageNumber());
	sExternalFlash.dataLen = testLength;
	sExternalFlash.ReadData();
	for(i = 0; i < testLength; i++){
		if(sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i] != 0x55)
			sTextOutput.FormatAndOutputString(" error:%d=%x", i, sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i]);
	}
	sTextOutput.FormatAndOutputString(" done.\r\n");

	sTextOutput.FormatAndOutputString("Writing 0xAA in page %d\r\n", sExternalFlash.GetPageNumber());
	for(i = 0; i < testLength; i++)
		sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i] = 0xAA;
	sExternalFlash.dataLen = testLength;
	sExternalFlash.WriteData();

	sTextOutput.FormatAndOutputString("Check if page %d contains 0x00:", sExternalFlash.GetPageNumber());
	sExternalFlash.dataLen = testLength;
	sExternalFlash.ReadData();
	for(i = 0; i < testLength; i++){
		if(sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i] != 0x00)
			sTextOutput.FormatAndOutputString(" error:%d=%x", i, sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + i]);
	}
	sTextOutput.FormatAndOutputString(" done.");

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestCRC(void)
{
	bool res;

	sTextOutput.InitBuffer();

	sCRC.Enable();

	sCRC.AddData8(0x31);
	sTextOutput.FormatAndOutputString("1: %8x\r\n", sCRC.GetData());
	res  = sCRC.GetData() == 0x83dcefb7;

	sCRC.AddData8(0x32);
	sTextOutput.FormatAndOutputString("12: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0x4f5344cd;

	sCRC.AddData8(0x33);
	sTextOutput.FormatAndOutputString("123: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0x884863d2;

	sCRC.AddData8(0x34);
	sTextOutput.FormatAndOutputString("1234: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0x9be3e0a3;

	sCRC.AddData32(0x35363738);
	sTextOutput.FormatAndOutputString("12345678: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0x9ae0daaf;

	sCRC.AddData8(0x39);
	sTextOutput.FormatAndOutputString("123456789: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0xcbf43926;

	sCRC.Disable();

	sTextOutput.FormatAndOutputString("CRC Test 1: ");
	if(res) sTextOutput.FormatAndOutputString("passed.\r\n");
	else    sTextOutput.FormatAndOutputString("failed !\r\n");

	sCRC.Enable();
	sCRC.AddData32(0x31323334);
	sCRC.AddData32(0x35363738);
	sTextOutput.FormatAndOutputString("12345678: %8x\r\n", sCRC.GetData());
	res  = sCRC.GetData() == 0x9ae0daaf;
	sCRC.AddData8(0x39);
	sTextOutput.FormatAndOutputString("123456789: %8x\r\n", sCRC.GetData());
	res &= sCRC.GetData() == 0xcbf43926;
	sCRC.Disable();

	sTextOutput.FormatAndOutputString("CRC Test 2: ");
	if(res) sTextOutput.FormatAndOutputString("passed.\r\n");
	else    sTextOutput.FormatAndOutputString("failed !\r\n");

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------

void BoardTester::TestPAWPackets(void)
{
	PAx5CommProtocol::PAW_CommProtocol pawc;
	PAx5CommProtocol::CryptoContext encCtx;
	uint8_t i, dataLen;
	bool res;

	sTextOutput.InitBuffer();

	pawc.Initialize(7);
	sTextOutput.FormatAndOutputString("128-bits Key:");
	for(i = 0; i < 4; i++){
		encCtx.key[i] = entropy.Get32bits();
		sTextOutput.FormatAndOutputString(" %8x", encCtx.key[i]);
	}
	sTextOutput.FormatAndOutputString("\r\n");

	dataLen = 27;
	for(i = 0; i < dataLen; i++)
		pawc.packetTX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + i] = entropy.Get8bits();
	entropy.Disable();
	pawc.BuildPacketHeader(&encCtx, 0x11, 0x22, dataLen, 0x12345678);

	sTextOutput.FormatAndOutputString("PT:");
	for(i = 0; i < pawc.packetTX[0]; i++)
		sTextOutput.FormatAndOutputString(" %2x", pawc.packetTX[i]);
	sTextOutput.FormatAndOutputString("\r\n");

	pawc.EncryptAndSign(&encCtx);

	for(i = 0; i < PAx5CommProtocol::CP_PACKET_MAX_LEN; i++)
		pawc.packetRX[i] = pawc.packetTX[i];

	res = pawc.CheckSignatureAndDecrypt(&encCtx);

	sTextOutput.FormatAndOutputString("DT:");
	for(i = 0; i < pawc.packetRX[0]; i++)
		sTextOutput.FormatAndOutputString(" %2x", pawc.packetRX[i]);
	sTextOutput.FormatAndOutputString("\r\n");

	sTextOutput.FormatAndOutputString("ET:");
	for(i = 0; i < pawc.packetTX[0]; i++)
		sTextOutput.FormatAndOutputString(" %2x", pawc.packetTX[i]);
	sTextOutput.FormatAndOutputString("\r\n");

	if(res) sTextOutput.FormatAndOutputString("Signature check OK.\r\n");
	else    sTextOutput.FormatAndOutputString("Signature check failed !\r\n");

	sTextOutput.Flush();
}

// -----------------------------------------------------------------------------
} /* namespace */
