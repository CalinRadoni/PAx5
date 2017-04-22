/**
 * created 2016.02.21 by Calin Radoni
 */

#ifndef dev_RFM69_H_
#define dev_RFM69_H_

#include "cpu_SPI1.h"

namespace PAx5 {

#define RFM69_RegFifo          0x00 // FIFO read/write access (reset: 0x00)
#define RFM69_RegOpMode        0x01 // Operating modes of the transceiver (reset: 0x04)
#define RFM69_RegDataModul     0x02 // Data operation mode and Modulation settings (reset: 0x00)
#define RFM69_RegBitrateMsb    0x03 // Bit Rate setting, Most Significant Bits (reset: 0x1A)
#define RFM69_RegBitrateLsb    0x04 // Bit Rate setting, Least Significant Bits (reset: 0x0B)
#define RFM69_RegFdevMsb       0x05 // Frequency Deviation setting, Most Significant Bits (reset: 0x00)
#define RFM69_RegFdevLsb       0x06 // Frequency Deviation setting, Least Significant Bits (reset: 0x52)
#define RFM69_RegFrfMsb        0x07 // RF Carrier Frequency, Most Significant Bits (reset: 0xE4)
#define RFM69_RegFrfMid        0x08 // RF Carrier Frequency, Intermediate Bits (reset: 0xC0)
#define RFM69_RegFrfLsb        0x09 // RF Carrier Frequency, Least Significant Bits (reset: 0x00)
#define RFM69_RegOsc1          0x0A // RC Oscillators Settings (reset: 0x41)
#define RFM69_RegAfcCtrl       0x0B // AFC control in low modulation index situations (reset: 0x00)
#define RFM69_Reserved0C       0x0C // (reset: 0x02)
#define RFM69_RegListen1       0x0D // Listen Mode settings (reset: 0x92)
#define RFM69_RegListen2       0x0E // Listen Mode Idle duration (reset: 0xF5)
#define RFM69_RegListen3       0x0F // Listen Mode Rx duration (reset: 0x20)
#define RFM69_RegVersion       0x10 // Module version (reset: 0x24)
#define RFM69_RegPaLevel       0x11 // PA selection and Output Power control (reset: 0x9F)
#define RFM69_RegPaRamp        0x12 // Control of the PA ramp time in FSK mode (reset: 0x09)
#define RFM69_RegOcp           0x13 // Over Current Protection control (reset: 0x1A)
#define RFM69_Reserved14       0x14 // (reset: 0x40)
#define RFM69_Reserved15       0x15 // (reset: 0xB0)
#define RFM69_Reserved16       0x16 // (reset: 0x7B)
#define RFM69_Reserved17       0x17 // (reset: 0x9B)
#define RFM69_RegLna           0x18 // LNA settings (reset: 0x08 recommended: 0x88)
#define RFM69_RegRxBw          0x19 // Channel Filter BW Control (reset: 0x86 recommended: 0x55)
#define RFM69_RegAfcBw         0x1A // Channel Filter BW control during the AFC routine (reset: 0x8A recommended: 0x8B)
#define RFM69_RegOokPeak       0x1B // OOK demodulator selection and control in peak mode (reset: 0x40)
#define RFM69_RegOokAvg        0x1C // Average threshold control of the OOK demodulator (reset: 0x80)
#define RFM69_RegOokFix        0x1D // Fixed threshold control of the OOK demodulator (reset: 0x06)
#define RFM69_RegAfcFei        0x1E // AFC and FEI control and status (reset: 0x10)
#define RFM69_RegAfcMsb        0x1F // MSB of the frequency correction of the AFC (reset: 0x00)
#define RFM69_RegAfcLsb        0x20 // LSB of the frequency correction of the AFC (reset: 0x00)
#define RFM69_RegFeiMsb        0x21 // MSB of the calculated frequency error (reset: 0x00)
#define RFM69_RegFeiLsb        0x22 // LSB of the calculated frequency error (reset: 0x00)
#define RFM69_RegRssiConfig    0x23 // RSSI-related settings (reset: 0x02)
#define RFM69_RegRssiValue     0x24 // RSSI value in dBm (reset: 0xFF)
#define RFM69_RegDioMapping1   0x25 // Mapping of pins DIO0 to DIO3 (reset: 0x00)
#define RFM69_RegDioMapping2   0x26 // Mapping of pins DIO4 and DIO5, ClkOut frequency (reset: 0x05 recommended: 0x07)
#define RFM69_RegIrqFlags1     0x27 // Status register: PLL Lock state, Timeout, RSSI > Threshold... (reset: 0x80)
#define RFM69_RegIrqFlags2     0x28 // Status register: FIFO handling flags... (reset: 0x00)
#define RFM69_RegRssiThresh    0x29 // RSSI Threshold control (reset: 0xFF recommended: 0xE4)
#define RFM69_RegRxTimeout1    0x2A // Timeout duration between Rx request and RSSI detection (reset: 0x00)
#define RFM69_RegRxTimeout2    0x2B // Timeout duration between RSSI detection and PayloadReady (reset: 0x00)
#define RFM69_RegPreambleMsb   0x2C // Preamble length, MSB (reset: 0x00)
#define RFM69_RegPreambleLsb   0x2D // Preamble length, LSB (reset: 0x03)
#define RFM69_RegSyncConfig    0x2E // Sync Word Recognition control (reset: 0x98)
#define RFM69_RegSyncValue1    0x2F // Sync Word byte 1 (reset: 0x00)
#define RFM69_RegSyncValue2    0x30 // Sync Word byte 2 (reset: 0x00)
#define RFM69_RegSyncValue3    0x31 // Sync Word byte 3 (reset: 0x00)
#define RFM69_RegSyncValue4    0x32 // Sync Word byte 4 (reset: 0x00)
#define RFM69_RegSyncValue5    0x33 // Sync Word byte 5 (reset: 0x00)
#define RFM69_RegSyncValue6    0x34 // Sync Word byte 6 (reset: 0x00)
#define RFM69_RegSyncValue7    0x35 // Sync Word byte 7 (reset: 0x00)
#define RFM69_RegSyncValue8    0x36 // Sync Word byte 8 (reset: 0x00)
#define RFM69_RegPacketConfig1 0x37 // Packet mode settings (reset: 0x10)
#define RFM69_RegPayloadLength 0x38 // Payload length setting (reset: 0x40)
#define RFM69_RegNodeAdrs      0x39 // Node address (reset: 0x00)
#define RFM69_RegBroadcastAdrs 0x3A // Broadcast address (reset: 0x00)
#define RFM69_RegAutoModes     0x3B // Auto modes settings (reset: 0x00)
#define RFM69_RegFifoThresh    0x3C // Fifo threshold, Tx start condition (reset: 0x0F recommended: 0x8F)
#define RFM69_RegPacketConfig2 0x3D // Packet mode settings (reset: 0x02)
#define RFM69_RegAesKey1       0x3E // Cypher key byte 1 (reset: 0x00)
#define RFM69_RegAesKey2       0x3F // Cypher key byte 2 (reset: 0x00)
#define RFM69_RegAesKey3       0x40 // Cypher key byte 3 (reset: 0x00)
#define RFM69_RegAesKey4       0x41 // Cypher key byte 4 (reset: 0x00)
#define RFM69_RegAesKey5       0x42 // Cypher key byte 5 (reset: 0x00)
#define RFM69_RegAesKey6       0x43 // Cypher key byte 6 (reset: 0x00)
#define RFM69_RegAesKey7       0x44 // Cypher key byte 7 (reset: 0x00)
#define RFM69_RegAesKey8       0x45 // Cypher key byte 8 (reset: 0x00)
#define RFM69_RegAesKey9       0x46 // Cypher key byte 9 (reset: 0x00)
#define RFM69_RegAesKey10      0x47 // Cypher key byte 10 (reset: 0x00)
#define RFM69_RegAesKey11      0x48 // Cypher key byte 11 (reset: 0x00)
#define RFM69_RegAesKey12      0x49 // Cypher key byte 12 (reset: 0x00)
#define RFM69_RegAesKey13      0x4A // Cypher key byte 13 (reset: 0x00)
#define RFM69_RegAesKey14      0x4B // Cypher key byte 14 (reset: 0x00)
#define RFM69_RegAesKey15      0x4C // Cypher key byte 15 (reset: 0x00)
#define RFM69_RegAesKey16      0x4D // Cypher key byte 16 (reset: 0x00)
#define RFM69_RegTemp1         0x4E // Temperature Sensor control (reset: 0x01)
#define RFM69_RegTemp2         0x4F // Temperature readout (reset: 0x00)
#define RFM69_RegTestLna       0x58 // Sensitivity boost (reset: 0x1B)
#define RFM69_RegTestPa1       0x5A // High Power PA settings (reset: 0x55)
#define RFM69_RegTestPa2       0x5C // High Power PA settings (reset: 0x70)
#define RFM69_RegTestDagc      0x6F // Fading Margin Improvement (reset: 0x00 recommended: 0x30)
#define RFM69_RegTestAfc       0x71 // AFC offset for low modulation index AFC (reset: 0x00)

#define RFM69_FIFO_SIZE 66
#define RFM69_MAX_MSG_LEN 64

#define RFM69_RadioBufferOffset 2 ///< needed to transfer data through SPI

#if ((RFM69_MAX_MSG_LEN + RFM69_RadioBufferOffset) > RFM69_FIFO_SIZE)
	#error "RFM69_MAX_MSG_LEN must be reduced !"
#endif

enum RadioMode {
	RM_Sleep, RM_StandBy, RM_FS, RM_TX, RM_RX
};

enum RadioModulation {
	RMO_FSK, RMO_GFSK, RMO_OOK
};

enum RadioDCFree {
	RDCF_None, RDCF_Whitening, RDCF_Mancester
};

enum RadioBitRate {
	RBR_5k     = 0,
	RBR_20k    = 1,
	RBR_25k    = 2,
	RBR_50k    = 3,
	RBR_100kM1 = 4,
	RBR_100kM2 = 5,
	RBR_125kM1 = 6,
	RBR_125kM2 = 7,
	RBR_250k   = 8
};

class DEV_RFM69
{
protected:
	bool configured;

	uint32_t timeStart; ///< used to wait in various functions

	//uint8_t ReadRegister(uint8_t);
	void WriteRegister(uint8_t, uint8_t);

	void ConfigureInterrupt(void);

	uint8_t nodeAddress;

	volatile RadioMode radioMode;
	uint32_t  radioFreq;
	volatile int8_t    outputPower;
	uint8_t   paSelection;
	void SetPowerAmplifiers(void);

	void ReadFIFO(void);

	/** @brief Packet length adjustment in variable length mode
	 *
	 * In variable length mode the payload starts with length field (1 byte length).
	 * For RFM69 the length is the length of payload without the length byte.
	 *
	 * If changePacketLength is true:
	 *    - before sending the packet packet[0]--
	 *    - after receiving the packet packet[0]++
	 *
	 * Default value is false but will be set equal to initVariableLengthPacketFormat
	 * when calling the Initialize function.
	 */
	volatile bool changePacketLength;

public:
	DEV_RFM69();
	virtual ~DEV_RFM69();

	uint8_t ReadRegister(uint8_t);

	RadioModulation initRM;                         // default: RMO_FSK
	RadioDCFree     initDCF;                        // default: RDCF_None
	RadioBitRate    initBR;                         // default: RBR_100kM1
	bool            initUseAFC;                     // default: true
	bool            initFilterAddress;              // default: true
	bool            initVariableLengthPacketFormat; // default: true
	void Initialize(void);
	void SetNodeAddress(uint8_t);

	void SetMode(RadioMode);
	void SetFrequency(uint32_t);
	uint32_t GetFrequency(void);

	// usable interval: -18dBm to +17dBm
	void SetPowerOutput(int8_t);

	volatile bool    packetReceived;
	volatile uint8_t packetReceivedLen;
	volatile int8_t  packetReceivedRSSI;

	volatile uint8_t radioBuffer[RFM69_FIFO_SIZE];

	bool IsFIFOEmpty(void);
	void ClearFIFO(void);

	/**
	 * After a packet is received and copied from FIFO to radioBuffer the mode will be set to RM_StandBy
	 * if switchRX2RX is true the mode will be set back to RM_RX.
	 * Default value: true
	 */
	volatile bool switchRX2RX;

	/** Time interval to wait for channel to be free
	 *
	 * If waitTimeForFreeAir > 0, CSMA/CA algorithm is enabled.
	 * This should not add more then 2ms to send time.
	 *
	 * During this time interval received data will be lost.
	 *
	 * Default value: 300 ms for a gateway.
	 */
	uint32_t waitTimeForFreeAir;

	/** Wait for channel to be free
	 *
	 * @attention This function puts the radio in RX mode.
	 * Before exiting the function will put the radio in Standby mode.
	 *
	 * @return true if the channel is free
	 */
	bool WaitForFreeAir(void);

	/**
	 * Send the packet, starting from radioBuffer[RFM69_RadioBufferOffset].
	 * The parameter is the number of bytes to send.
	 *
	 * This function ignores the waitTimeForFreeAir variable.
	 *
	 * @return false if number of bytes to be sent is greater then RFM69_MAX_MSG_LEN.
	 */
	bool SendMessage(uint8_t);

	/**
	 * Send the packet, starting from the buffer passed as parameter.
	 * The data MUST be in the buffer starting from the third byte.
	 * The first and second bytes will be changed !
	 *
	 * The second parameter is the number of bytes to send.
	 *
	 * This function calls the WaitForFreeAir function if waitTimeForFreeAir > 0.
	 *
	 * @return false if number of bytes to be sent is greater then RFM69_MAX_MSG_LEN.
	 * @return false if waitTimeForFreeAir > 0 and the channel is not free in that interval of time.
	 */
	bool SendMessage(uint8_t*, uint8_t);

	/**
	 * Before sending a pachet is sent if(radioMode == RM_TX) SetMode(switchTX2FS ? RM_FS : RM_StandBy) - clears the FIFO
	 * After a packet is sent SetMode(switchTX2FS ? RM_FS : RM_StandBy) - clears the FIFO
	 * Default value: false
	 */
	volatile bool switchTX2FS;

	/**
	 * Set in interrupt handler when the FIFO content is sent
	 */
	volatile bool packetSent;

	void ReadRSSI(void);

	// set in Initialize, FillFIFO, HandleInterrupt,
	volatile bool interfaceError;

	void HandleInterrupt(void);
};

extern DEV_RFM69 sRadio;

} /* namespace */
#endif /* dev_RFM69_H_ */
