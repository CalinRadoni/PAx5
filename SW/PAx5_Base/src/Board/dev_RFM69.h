/**
 * created 2016.02.21 by Calin Radoni
 */

#ifndef dev_RFM69_H_
#define dev_RFM69_H_

#include "cpu_SPI1.h"

namespace PAx5 {

const uint8_t RFM69_FIFO_Size = 66;
const uint8_t RFM69_MaxMsgLen = 64;
const uint8_t RFM69_RadioBufferOffset = 2; ///< needed to transfer data through SPI

static_assert((RFM69_MaxMsgLen + RFM69_RadioBufferOffset) <= RFM69_FIFO_Size, "RFM69_MaxMsgLen must be reduced !");

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
public:
	DEV_RFM69();
	virtual ~DEV_RFM69();

	RadioModulation initRM;                         // default: RMO_FSK
	RadioDCFree     initDCF;                        // default: RDCF_None
	RadioBitRate    initBR;                         // default: RBR_100kM1
	bool            initUseAFC;                     // default: true
	bool            initFilterAddress;              // default: true
	bool            initVariableLengthPacketFormat; // default: true
	void Initialize(void);

	/**
	 * \brief Set the address of this node
	 * \details It also applies:
	 *    - initVariableLengthPacketFormat
	 *    - initDCF
	 *    - initFilterAddress
	 */
	void SetNodeAddress(uint8_t);

	/**
	 * \brief Get the version of RFM69 module
	 * \details bits[7:4] = rev.number
	 *          bits[3:0] = metal mask rev.no.
	 */
	uint8_t GetModuleVersion(void);

	/**
	 * \brief Test read and write to a sync register
	 * \details Write #val in the sync register number #syncRegNo then
	 *          read the register and return that value.
	 * \note This function is used to 'test' reading and writing to
	 *       radio module's registers.
	 */
	uint8_t TestSyncRegister(uint8_t syncRegNo, uint8_t val); ///< syncRegNo = {0 - 3}

	void SetMode(RadioMode);
	void SetFrequency(uint32_t);
	uint32_t GetFrequency(void);

	// usable interval: -18dBm to +17dBm
	void SetPowerOutput(int8_t);

	volatile bool    packetReceived;
	volatile uint8_t packetReceivedLen;
	volatile int8_t  packetReceivedRSSI;

	volatile uint8_t radioBuffer[RFM69_FIFO_Size];

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
	 * Before sending a packet is sent if(radioMode == RM_TX) SetMode(switchTX2FS ? RM_FS : RM_StandBy) - clears the FIFO
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

protected:
	bool configured;

	uint32_t timeStart; ///< used to wait in various functions

	uint8_t ReadRegister(uint8_t);
	void WriteRegister(uint8_t, uint8_t);

	void ConfigureInterrupt(void);

	uint8_t nodeAddress;

	volatile RadioMode radioMode;
	uint32_t  radioFreq;
	volatile int8_t    outputPower;
	uint8_t   paSelection;
	void SetPowerAmplifiers(void);

	void ReadFIFO(void);

	/**
	 * \brief Packet length adjustment in variable length mode
	 *
	 * \details In variable length mode the payload starts with length field (1 byte length).
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
};

extern DEV_RFM69 sRadio;

} /* namespace */
#endif /* dev_RFM69_H_ */
