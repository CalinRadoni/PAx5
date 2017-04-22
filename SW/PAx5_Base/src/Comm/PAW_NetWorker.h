/**
 * created 2016.12.05 by Calin Radoni
 */

#ifndef GW_NETWORKER_H_
#define GW_NETWORKER_H_

#include "PAW_Defines.h"
#include "PAW_CommProtocol.h"

namespace PAx5CommProtocol {

const uint8_t GW_MAX_COMM_SLOTS = 10;

class PAW_NetWorker {
private:

	PAW_TimedSlot commSlots[GW_MAX_COMM_SLOTS];

	uint32_t timeStart;

public:
	PAW_NetWorker();

	PAW_TimedSlot* GetFreeSlot(uint8_t);
	PAW_TimedSlot* GetSlot(uint8_t, uint32_t, PAW_TimedSlot::PAW_TS_State);

	enum PAW_RXCheckResult {
		chk_PacketOK        =  0, // OK, received a data packet

		chk_RxLenHigh       =  1, // fail
		chk_RxLenDifferent  =  2, // fail
		chk_RxLenLow        =  3, // fail
		chk_WrongAddr       =  4, // fail
		chk_AddrOther       =  5, // not for me, ignore

		chk_AddrRequest     = 10, // OK ??????? Assign address

		chk_Bcast           = 11, // broadcast, not defined/implemented yet

		chk_WronkPacketLen  = 20, // fail, invalid packet length (based on packet type)
		chk_CommSlotsFull   = 21, // fail
		chk_SignatureFailed = 22, // fail, packet's signature is invalid
		chk_ChannelNotFree  = 23, // fail
		chk_ReqAckSent      = 24, // OK, packet sent

		chk_NotMyPeer       = 25, // fail
		chk_WrongSessionID  = 26, // fail

		chk_ReqAckNoReq     = 27, // fail, received REQ+ACK but NO REQ sent
		chk_ReqAckReceived  = 28, // OK, REQ+ACK received for commProtocol.serverSlot

		chk_AckNoAck        = 29, // fail, received ACK but not waiting for one
		chk_AckReceived     = 30, // OK, ACK received, update commProtocol.serverSlot if more data to send

		chk_DataNotPeer     = 31, // fail, received data but not from a registered peer

		chk_End             = 0x7F // not used
	};

	PAW_CommProtocol commProtocol;
	CryptoContext netCtx;

	void Initialize(uint8_t nodeAddress, uint32_t frequency);
	void DecreaseTimeSlots(uint32_t ms);

	PAW_TimedSlot* dataPeer;
	PAW_RXCheckResult CheckReceivedPacket(void);

	bool SendPacket(void);
};

} /* namespace */

#endif
