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

#include <NetWorker.h>
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

NetWorker::NetWorker()
{
}

void NetWorker::Initialize(uint8_t nodeAddress, uint32_t frequency)
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

void NetWorker::DecreaseTimeSlots(uint32_t ms)
{
	uint8_t i;

	for(i = 0; i < GW_MAX_COMM_SLOTS; i++)
		commSlots[i].DecreaseTime(ms);
}

TimedSlot* NetWorker::GetFreeSlot(uint8_t srcAddr)
{
	uint8_t i, res;
	bool found;

	res = GW_MAX_COMM_SLOTS;
	found = false;
	for(i = 0; i < GW_MAX_COMM_SLOTS; i++){
		if(commSlots[i].state == TimedSlot::PAW_TS_State::stateIdle) res = i;
		else{
			if(commSlots[i].peerAddress == srcAddr)
				found = true;
		}
	}
	if(found) return NULL;

	if(res < GW_MAX_COMM_SLOTS) return &commSlots[res];
	return NULL;
}

TimedSlot* NetWorker::GetSlot(uint8_t srcAddr, uint32_t packetID, TimedSlot::PAW_TS_State slotState)
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

NetWorker::RXCheck NetWorker::CheckReceivedPacket(void)
{
	uint8_t flags, packLen, srcAddr;
	uint32_t dataH, dataL, packetID;
	uint8_t i, *ptr;
	TimedSlot* nodeTS;

	if(PAx5::sRadio.packetReceivedLen > CP_PACKET_MAX_LEN) return RXCheck::RxLenHigh; ///< packet too long
	packLen = PAx5::sRadio.radioBuffer[CP_PACKET_HDR_Length_POS + PAx5::RFM69_RadioBufferOffset];
	if(PAx5::sRadio.packetReceivedLen != packLen) return RXCheck::RxLenDifferent; ///< length != received length
	if(packLen < CP_PACKET_OVERHEAD) return RXCheck::RxLenLow; ///< packet too short

	ptr = (uint8_t*)&PAx5::sRadio.radioBuffer[PAx5::RFM69_RadioBufferOffset];
	// copy the packet from sRadio
	for(i = 0; i < packLen; i++)
		commProtocol.packetRX[i] = *ptr++;

	CommProtocol::AddrChk checkAddr = commProtocol.CheckAddresses();
	if(checkAddr == CommProtocol::AddrChk::wrongCombo) return RXCheck::WrongAddr;    ///< addresses are wrong or not for this node

	flags    = commProtocol.packetRX[CP_PACKET_HDR_FLAGS_POS] & CP_PACKET_HDR_FLAGS;
	srcAddr  = commProtocol.packetRX[CP_PACKET_HDR_SrcAddr_POS];
	packetID = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HDR_ID_POS]);

	if(checkAddr == CommProtocol::AddrChk::srcNone_dstGW){
		// TODO Check if it is an address request packet and process it if it is.
		return RXCheck::AddrRequest;
	}

	if(checkAddr == CommProtocol::AddrChk::srcOK_dstBcast) {
		// TODO Process broadcast packet. Define them first !
		return RXCheck::Bcast;
	}

	if(commProtocol.nodeAddress == AddressClass::Address_Gateway){
		if((checkAddr != CommProtocol::AddrChk::srcOK_dstME) && (checkAddr != CommProtocol::AddrChk::srcOK_dstGW)){
			return RXCheck::AddrOther;
		}
	}
	else{
		if(checkAddr != CommProtocol::AddrChk::srcOK_dstME) {
			return RXCheck::AddrOther;
		}
	}

	// this packet is for this node, must be processed

	if(flags == CP_PACKET_HDR_REQ){
		// this is a request packet

		// the packet should contain at least the client nonce
		if(packLen < (CP_PACKET_OVERHEAD + 8)) return RXCheck::WronkPacketLen;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&netCtx)) return RXCheck::SignatureFailed;

		// get a free communication slot
		nodeTS = GetFreeSlot(srcAddr);
		if(nodeTS == NULL) return RXCheck::CommSlotsFull;

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
			return RXCheck::ChannelNotFree;
		return RXCheck::ReqAckSent;
	}

	if(flags == (CP_PACKET_HDR_REQ | CP_PACKET_HDR_ACK)){
		// get the communication slot
		nodeTS = GetSlot(srcAddr, packetID, TimedSlot::PAW_TS_State::stateWaitReqAck);
		if(nodeTS == NULL) return RXCheck::ReqAckNoReq;
		// is from my peer ?
		if(nodeTS->peerAddress != srcAddr) return RXCheck::NotMyPeer;
		// is the same session ID ?
		if(nodeTS->GetSessionID() != packetID) return RXCheck::WrongSessionID;

		// the packet should contain at least the server nonce
		if(packLen < (CP_PACKET_OVERHEAD + 8)) return RXCheck::WronkPacketLen;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&netCtx)) return RXCheck::SignatureFailed;

		// store peer's nonce
		dataH = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN]);
		dataL = commProtocol.BP2DW(&commProtocol.packetRX[CP_PACKET_HEADER_LEN + 4]);
		nodeTS->UpdateTimeSlotReqAck(CP_TIME_WAIT_REQACK, dataH, dataL);

		commProtocol.CreateSessionKey(nodeTS);

		dataPeer = nodeTS;
		return RXCheck::ReqAckReceived;
	}

	if(flags == CP_PACKET_HDR_ACK){
		// get the communication slot
		nodeTS = GetSlot(srcAddr, packetID, TimedSlot::PAW_TS_State::stateWaitAck);
		if(nodeTS == NULL) return RXCheck::AckNoAck;
		// is from my peer ?
		if(nodeTS->peerAddress != srcAddr) return RXCheck::NotMyPeer;
		// is the same session ID ?
		if(nodeTS->GetSessionID() != packetID) return RXCheck::WrongSessionID;

		// check signature and decrypt the packet
		if(!commProtocol.CheckSignatureAndDecrypt(&nodeTS->sessionEncCtx)) return RXCheck::SignatureFailed;

		dataPeer = nodeTS;
		return RXCheck::AckReceived;
	}

	// this should be a data packet for which a commSlot is in the stateHandshakeDone state
	nodeTS = GetSlot(srcAddr, packetID, TimedSlot::PAW_TS_State::stateHandshakeDone);
	if(nodeTS == NULL) return RXCheck::DataNotPeer;

	if(!commProtocol.CheckSignatureAndDecrypt(&nodeTS->sessionEncCtx)) return RXCheck::SignatureFailed;

	commProtocol.CreateAckPacket(nodeTS, 0);
	if(!SendPacket())
		return RXCheck::ChannelNotFree;

	if(flags == CP_PACKET_HDR_WAIT) nodeTS->UpdateTimeSlotAckReceived(CP_TIME_WAIT_DATA);
	else                            nodeTS->UpdateTimeSlotAckReceived(0);

	dataPeer = nodeTS;

	return RXCheck::PacketOK;
}

bool NetWorker::SendPacket(void)
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
