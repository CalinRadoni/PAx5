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

#ifndef TIMEDSLOT_H_
#define TIMEDSLOT_H_

#include <CommDefines.h>
#include <CryptoDefs.h>

namespace PAx5CommProtocol {

class TimedSlot {
protected:
	uint32_t timeLeft;

public:
	TimedSlot();

	enum class PAW_TS_State : uint8_t
	{
		stateIdle = 0,
		stateWaitReqAck = 1,
		stateHandshakeDone = 2,
		stateWaitAck = 3,
		stateAckReceived = 4
	};

	CryptoContext sessionEncCtx;
	uint8_t peerAddress;
	PAW_TS_State state;
	uint16_t packetNumber;

	/** Initialize the time slot and set client nonce
	 *
	 * if(ms == 0) state = stateIdle
	 * else        state = stateWaitReqAck
	 */
	void InitTimeSlot(uint32_t ms, uint32_t clientNonceH, uint32_t clientNonceL);

	/** Update the time slot and set server nonce
	 *
	 * if(ms == 0) state = stateIdle
	 * else        state = stateHandshakeDone
	 */
	void UpdateTimeSlotReqAck(uint32_t ms, uint32_t serverNonceH, uint32_t serverNonceL);

	/** Update the time slot
	 *
	 * state = stateWaitAck
	 */
	void UpdateTimeSlotAckWait(uint32_t ms);

	/** Update the time slot
	 *
	 * state = stateAckReceived
	 */
	void UpdateTimeSlotAckReceived(uint32_t ms);

	/** Set a new value for timeLeft
	 *
	 * state will be set to stateIdle if timeLeft == 0
	 */
	void RestartTimeSlot(uint32_t ms);

	/** Decrease time left
	 *
	 * state will be set to stateIdle if timeLeft == 0
	 */
	void DecreaseTime(uint32_t ms);

	uint32_t GetSessionID(void);
	void SetSessionID(uint32_t);
};

} /* namespace */

#endif
