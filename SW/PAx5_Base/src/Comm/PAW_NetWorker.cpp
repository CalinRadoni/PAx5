/**
 * created 2016.12.05 by Calin Radoni
 */

#include <PAW_NetWorker.h>
#include "dev_RFM69.h"
#include "cpu_Entropy.h"

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

//#define ProtocolNetWorker_SEND_Simulation

// -----------------------------------------------------------------------------

static_assert(PAx5::RFM69_RadioBufferOffset == CP_PACKET_RAW_OFFSET, "wrong offsets");

// -----------------------------------------------------------------------------

#define SEND_TIME_WAIT_FOR_FREE_AIR 100
#define SEND_TRY_COUNT 3

// -----------------------------------------------------------------------------

PAW_NetWorker::PAW_NetWorker()
{
}

void PAW_NetWorker::Initialize(uint8_t nodeAddress, uint32_t frequency)
{
	uint8_t i;

	PAx5::sRadio.SetMode(PAx5::RM_StandBy);
	PAx5::sRadio.switchRX2RX = false;
	PAx5::sRadio.switchTX2FS = false;
	PAx5::sRadio.waitTimeForFreeAir = SEND_TIME_WAIT_FOR_FREE_AIR;

	PAx5::sRadio.initFilterAddress = false;
	PAx5::sRadio.SetNodeAddress(nodeAddress);
	PAx5::sRadio.SetFrequency(frequency);

	switch(nodeAddress) {
		case AddressClass::Address_Gateway:
			PAx5::sRadio.SetPowerOutput(18);
			break;
		case AddressClass::Address_NONE:
			PAx5::sRadio.SetPowerOutput(1);
			break;
		default:
			PAx5::sRadio.SetPowerOutput(17);
			break;
	}

	commProtocol.Initialize(nodeAddress);

	for(i = 0; i < GW_MAX_COMM_SLOTS; i++)
		commSlots[i].RestartTimeSlot(0);
}

// -----------------------------------------------------------------------------

void PAW_NetWorker::DecreaseTimeSlots(uint32_t ms)
{
	uint8_t i;

	for(i = 0; i < GW_MAX_COMM_SLOTS; i++)
		commSlots[i].DecreaseTime(ms);
}

PAW_TimedSlot* PAW_NetWorker::GetFreeSlot(uint8_t srcAddr)
{
	uint8_t i, res;
	bool found;

	res = GW_MAX_COMM_SLOTS;
	found = false;
	for(i = 0; i < GW_MAX_COMM_SLOTS; i++){
		if(commSlots[i].state == PAW_TimedSlot::PAW_TS_State::stateIdle) res = i;
		else{
			if(commSlots[i].peerAddress == srcAddr)
				found = true;
		}
	}
	if(found) return NULL;

	if(res < GW_MAX_COMM_SLOTS) return &commSlots[res];
	return NULL;
}

PAW_TimedSlot* PAW_NetWorker::GetSlot(uint8_t srcAddr, uint32_t packetID, PAW_TimedSlot::PAW_TS_State slotState)
{
	uint8_t i;

	for(i = 0; i < GW_MAX_COMM_SLOTS; i++){
		if(commSlots[i].peerAddress == srcAddr){
			if(commSlots[i].GetSessionID() == packetID){
				if(commSlots[i].state == slotState)
					return &commSlots[i];
			}
		}
	}
	return NULL;
}

// -----------------------------------------------------------------------------

PAW_NetWorker::PAW_RXCheckResult PAW_NetWorker::CheckReceivedPacket(void)
{
	uint8_t flags, packLen, checkAddr, srcAddr;
	uint32_t dataH, dataL, packetID;
	uint8_t i, *ptr;
	PAW_TimedSlot* nodeTS;

	if(PAx5::sRadio.packetReceivedLen > CP_PACKET_MAX_LEN) return PAW_RXCheckResult::chk_RxLenHigh; ///< packet too long
	packLen = PAx5::sRadio.radioBuffer[CP_PACKET_HDR_Length_POS + PAx5::RFM69_RadioBufferOffset];
	if(PAx5::sRadio.packetReceivedLen != packLen) return PAW_RXCheckResult::chk_RxLenDifferent; ///< length != received length
	if(packLen < CP_PACKET_OVERHEAD) return PAW_RXCheckResult::chk_RxLenLow; ///< packet too short

	ptr = (uint8_t*)&PAx5::sRadio.radioBuffer[PAx5::RFM69_RadioBufferOffset];
	// copy the packet from sRadio
	for(i = 0; i < packLen; i++)
		commProtocol.packetRX[i] = *ptr++;

	checkAddr = commProtocol.CheckAddresses();
	if(checkAddr == PAW_ChkAddr::wrongCombo) return PAW_RXCheckResult::chk_WrongAddr;    ///< addresses are wrong or not for this node

	flags    = commProtocol.packetRX[CP_PACKET_HDR_FLAGS_POS] & CP_PACKET_HDR_FLAGS;
	srcAddr  = commProtocol.packetRX[CP_PACKET_HDR_SrcAddr_POS];
	packetID = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HDR_ID_POS]);

	if(checkAddr == PAW_ChkAddr::srcNone_dstGW){
		// TODO Check if it is an address request packet and process it if it is.
		return PAW_RXCheckResult::chk_AddrRequest;
	}

	if(checkAddr == PAW_ChkAddr::srcOK_dstBcast) {
		// TODO Process broadcast packet. Define them first !
		return PAW_RXCheckResult::chk_Bcast;
	}

	if(commProtocol.nodeAddress == AddressClass::Address_Gateway){
		if((checkAddr != PAW_ChkAddr::srcOK_dstME) && (checkAddr != PAW_ChkAddr::srcOK_dstGW)){
			return PAW_RXCheckResult::chk_AddrOther;
		}
	}
	else{
		if(checkAddr != PAW_ChkAddr::srcOK_dstME) {
			return PAW_RXCheckResult::chk_AddrOther;
		}
	}

	// this packet is for this node, must be processed

	if(flags == CP_PACKET_HDR_REQ){
		// this is a request packet

		// the packet should contain at least the client nonce
		if(packLen < (CP_PACKET_OVERHEAD + 8)) return PAW_RXCheckResult::chk_WronkPacketLen;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&netCtx)) return PAW_RXCheckResult::chk_SignatureFailed;

		// get a free communication slot
		nodeTS = GetFreeSlot(srcAddr);
		if(nodeTS == NULL) return PAW_RXCheckResult::chk_CommSlotsFull;

		// store client ID and address
		nodeTS->peerAddress = srcAddr;
		nodeTS->SetSessionID(packetID);

		// store client nonce
		dataH = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN]);
		dataL = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN + 4]);
		nodeTS->InitTimeSlot(CP_TIME_WAIT_REQACK, dataH, dataL);

		// generate the server nonce, store that then create the packet
		commProtocol.CreateReqAckPacket(nodeTS, &netCtx);

		commProtocol.CreateSessionKey(nodeTS);

		if(!SendPacket())
			return PAW_RXCheckResult::chk_ChannelNotFree;
		return PAW_RXCheckResult::chk_ReqAckSent;
	}

	if(flags == (CP_PACKET_HDR_REQ | CP_PACKET_HDR_ACK)){
		// get the communication slot
		nodeTS = GetSlot(srcAddr, packetID, PAW_TimedSlot::PAW_TS_State::stateWaitReqAck);
		if(nodeTS == NULL) return PAW_RXCheckResult::chk_ReqAckNoReq;
		// is from my peer ?
		if(nodeTS->peerAddress != srcAddr) return PAW_RXCheckResult::chk_NotMyPeer;
		// is the same session ID ?
		if(nodeTS->GetSessionID() != packetID) return PAW_RXCheckResult::chk_WrongSessionID;

		// the packet should contain at least the server nonce
		if(packLen < (CP_PACKET_OVERHEAD + 8)) return PAW_RXCheckResult::chk_WronkPacketLen;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&netCtx)) return PAW_RXCheckResult::chk_SignatureFailed;

		// store peer's nonce
		dataH = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN]);
		dataL = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN + 4]);
		nodeTS->UpdateTimeSlotReqAck(CP_TIME_WAIT_REQACK, dataH, dataL);

		commProtocol.CreateSessionKey(nodeTS);

		dataPeer = nodeTS;
		return PAW_RXCheckResult::chk_ReqAckReceived;
	}

	if(flags == CP_PACKET_HDR_ACK){
		// get the communication slot
		nodeTS = GetSlot(srcAddr, packetID, PAW_TimedSlot::PAW_TS_State::stateWaitAck);
		if(nodeTS == NULL) return PAW_RXCheckResult::chk_AckNoAck;
		// is from my peer ?
		if(nodeTS->peerAddress != srcAddr) return PAW_RXCheckResult::chk_NotMyPeer;
		// is the same session ID ?
		if(nodeTS->GetSessionID() != packetID) return PAW_RXCheckResult::chk_WrongSessionID;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&nodeTS->sessionEncCtx)) return PAW_RXCheckResult::chk_SignatureFailed;

		dataPeer = nodeTS;
		return PAW_RXCheckResult::chk_AckReceived;
	}

	// this should be a data packet for which a commSlot is in the stateHandshakeDone state
	nodeTS = GetSlot(srcAddr, packetID, PAW_TimedSlot::PAW_TS_State::stateHandshakeDone);
	if(nodeTS == NULL) return PAW_RXCheckResult::chk_DataNotPeer;

	if(!commProtocol.CheckSignatureAndDecrypt(&nodeTS->sessionEncCtx)) return PAW_RXCheckResult::chk_SignatureFailed;

	commProtocol.CreateAckPacket(nodeTS, 0);
	if(!SendPacket())
		return PAW_RXCheckResult::chk_ChannelNotFree;

	if(flags == CP_PACKET_HDR_WAIT) nodeTS->UpdateTimeSlotAckReceived(CP_TIME_WAIT_DATA);
	else                            nodeTS->UpdateTimeSlotAckReceived(0);

	dataPeer = nodeTS;

	return PAW_RXCheckResult::PacketOK;
}

bool PAW_NetWorker::SendPacket(void)
{
#ifdef ProtocolNetWorker_SEND_Simulation

	uint8_t i, len;

	len = commProtocol.packetTX[CP_PACKET_HDR_Length_POS];
	for(i = 0; i < (RFM69_RadioBufferOffset + len); i++)
		PAx5::sRadio.radioBuffer[i] = commProtocol.packetOut[i];
	PAx5::sRadio.packetReceivedLen = len;

	return true;

#else

	uint8_t i;

	i = 0;
	do {
		if(PAx5::sRadio.SendMessage((uint8_t*)commProtocol.packetOut, commProtocol.packetTX[CP_PACKET_HDR_Length_POS])) {
			while(!PAx5::sRadio.packetSent) {
				__NOP(); // wait for radio to send it's queued packet
			}
			return true;
		}
		i++;
	} while(i < SEND_TRY_COUNT);

	return false;

#endif
}

// -----------------------------------------------------------------------------
} /* namespace */
