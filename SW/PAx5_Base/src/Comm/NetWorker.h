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

#ifndef COMM_NETWORKER_H_
#define COMM_NETWORKER_H_

#include <CommDefines.h>
#include <CommProtocol.h>

namespace PAx5CommProtocol {

const uint8_t GW_MAX_COMM_SLOTS = 10;

class NetWorker {
private:

	TimedSlot commSlots[GW_MAX_COMM_SLOTS];

	uint32_t timeStart;

public:
	NetWorker();

	TimedSlot* GetFreeSlot(uint8_t);
	TimedSlot* GetSlot(uint8_t, uint32_t, TimedSlot::PAW_TS_State);

	enum class RXCheck : uint8_t
	{
		PacketOK        =  0, // OK, received a data packet

		RxLenHigh       =  1, // fail
		RxLenDifferent  =  2, // fail
		RxLenLow        =  3, // fail
		WrongAddr       =  4, // fail
		AddrOther       =  5, // not for me, ignore

		AddrRequest     = 10, // OK ??????? Assign address

		Bcast           = 11, // broadcast, not defined/implemented yet

		WronkPacketLen  = 20, // fail, invalid packet length (based on packet type)
		CommSlotsFull   = 21, // fail
		SignatureFailed = 22, // fail, packet's signature is invalid
		ChannelNotFree  = 23, // fail
		ReqAckSent      = 24, // OK, packet sent

		NotMyPeer       = 25, // fail
		WrongSessionID  = 26, // fail

		ReqAckNoReq     = 27, // fail, received REQ+ACK but NO REQ sent
		ReqAckReceived  = 28, // OK, REQ+ACK received for commProtocol.serverSlot

		AckNoAck        = 29, // fail, received ACK but not waiting for one
		AckReceived     = 30, // OK, ACK received, update commProtocol.serverSlot if more data to send

		DataNotPeer     = 31 // fail, received data but not from a registered peer
	};

	CommProtocol commProtocol;
	CryptoContext netCtx;

	void Initialize(uint8_t nodeAddress, uint32_t frequency);
	void DecreaseTimeSlots(uint32_t ms);

	TimedSlot* dataPeer;
	RXCheck CheckReceivedPacket(void);

	bool SendPacket(void);
};

} /* namespace */

#endif
