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

#include <TimedSlot.h>

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

TimedSlot::TimedSlot()
{
	timeLeft = 0;
	peerAddress = AddressClass::Address_NONE;
	state = PAW_TS_State::stateIdle;
	packetNumber = 0;
}

void TimedSlot::InitTimeSlot(uint32_t ms, uint32_t clientNonceH, uint32_t clientNonceL)
{
	sessionEncCtx.key[0] = clientNonceH;
	sessionEncCtx.key[1] = clientNonceL;

	timeLeft = ms;
	if(timeLeft == 0) state = PAW_TS_State::stateIdle;
	else              state = PAW_TS_State::stateWaitReqAck;

	packetNumber = 0;
}

void TimedSlot::UpdateTimeSlotReqAck(uint32_t ms, uint32_t serverNonceH, uint32_t serverNonceL)
{
	sessionEncCtx.key[2] = serverNonceH;
	sessionEncCtx.key[3] = serverNonceL;

	timeLeft = ms;
	if(timeLeft == 0) state = PAW_TS_State::stateIdle;
	else              state = PAW_TS_State::stateHandshakeDone;

	packetNumber = 0;
}

void TimedSlot::UpdateTimeSlotAckWait(uint32_t ms)
{
	timeLeft = ms;
	state = PAW_TS_State::stateWaitAck;
}

void TimedSlot::UpdateTimeSlotAckReceived(uint32_t ms)
{
	timeLeft = ms;
	state = PAW_TS_State::stateAckReceived;
}

void TimedSlot::RestartTimeSlot(uint32_t ms)
{
	timeLeft = ms;
	if(timeLeft == 0)
		state = PAW_TS_State::stateIdle;
}

void TimedSlot::DecreaseTime(uint32_t ms)
{
	if(ms < timeLeft) timeLeft -= ms;
	else{
		timeLeft = 0;
		state = PAW_TS_State::stateIdle;
	}
}

uint32_t TimedSlot::GetSessionID(void)
{
	return sessionEncCtx.cnonce[1];
}

void TimedSlot::SetSessionID(uint32_t sessionID)
{
	sessionEncCtx.cnonce[1] = sessionID;
}

// -----------------------------------------------------------------------------
} /* namespace */
