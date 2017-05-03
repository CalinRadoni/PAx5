/**
 * created 2016.11.18 by Calin Radoni
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
