/**
 * created 2016.11.18 by Calin Radoni
 */

#include <PAW_TimedSlot.h>

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

PAW_TimedSlot::PAW_TimedSlot()
{
	timeLeft = 0;
	peerAddress = AddressClass::Address_NONE;
	state = PAW_TS_State::stateIdle;
	packetNumber = 0;
}

void PAW_TimedSlot::InitTimeSlot(uint32_t ms, uint32_t clientNonceH, uint32_t clientNonceL)
{
	sessionEncCtx.key[0] = clientNonceH;
	sessionEncCtx.key[1] = clientNonceL;

	timeLeft = ms;
	if(timeLeft == 0) state = PAW_TS_State::stateIdle;
	else              state = PAW_TS_State::stateWaitReqAck;

	packetNumber = 0;
}

void PAW_TimedSlot::UpdateTimeSlotReqAck(uint32_t ms, uint32_t serverNonceH, uint32_t serverNonceL)
{
	sessionEncCtx.key[2] = serverNonceH;
	sessionEncCtx.key[3] = serverNonceL;

	timeLeft = ms;
	if(timeLeft == 0) state = PAW_TS_State::stateIdle;
	else              state = PAW_TS_State::stateHandshakeDone;

	packetNumber = 0;
}

void PAW_TimedSlot::UpdateTimeSlotAckWait(uint32_t ms)
{
	timeLeft = ms;
	state = PAW_TS_State::stateWaitAck;
}

void PAW_TimedSlot::UpdateTimeSlotAckReceived(uint32_t ms)
{
	timeLeft = ms;
	state = PAW_TS_State::stateAckReceived;
}

void PAW_TimedSlot::RestartTimeSlot(uint32_t ms)
{
	timeLeft = ms;
	if(timeLeft == 0)
		state = PAW_TS_State::stateIdle;
}

void PAW_TimedSlot::DecreaseTime(uint32_t ms)
{
	if(ms < timeLeft) timeLeft -= ms;
	else{
		timeLeft = 0;
		state = PAW_TS_State::stateIdle;
	}
}

uint32_t PAW_TimedSlot::GetSessionID(void)
{
	return sessionEncCtx.cnonce[1];
}

void PAW_TimedSlot::SetSessionID(uint32_t sessionID)
{
	sessionEncCtx.cnonce[1] = sessionID;
}

// -----------------------------------------------------------------------------
} /* namespace */
