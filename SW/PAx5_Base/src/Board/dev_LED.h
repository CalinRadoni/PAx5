/**
 * created 2016.12.07 by Calin Radoni
 */

#ifndef DEV_LED_H_
#define DEV_LED_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class DEV_LED {
protected:
	uint8_t  mode;
	uint32_t timeOn;
	uint32_t timeOff;
	uint8_t  repeatCnt;

	bool ledON;
	uint32_t runTime;

public:
	DEV_LED();

	void On(void);
	void Off(void);

	/** Turns the LED on then off
	 *
	 * Blocking function.
	 *
	 * Turns the LED on for `ms` milliseconds then off for `ms` milliseconds.
	 * This function uses Delay function defined in MainBoard_Base
	 */
	void Blink(uint32_t ms);

	/**
	 * Non-blocking function.
	 * Turns the LED on for `ms` milliseconds.
	 * Decrease time left with the Pulse function.
	 */
	void ModeOn(uint32_t ms);

	/**
	 * Non-blocking function.
	 * Turns the LED on for `ms` milliseconds then off for `ms` milliseconds.
	 * Decrease time left with the Pulse function.
	 */
	void ModeBlink(uint32_t ms);

	/**
	 * Non-blocking function.
	 * Turns the LED on for `msOn` milliseconds then off for `msOff` milliseconds.
	 * Repeat the cycle `cnt` times.
	 * Decrease time left with the Pulse function.
	 */
	void ModeBlink(uint32_t msOn, uint32_t msOff, uint8_t cnt);

	/**
	 * Decrease time left of the current cycle with MIN(ms, time left)
	 * Returns true while current sequence is NOT completed.
	 */
	bool Pulse(uint32_t ms);
};

extern DEV_LED sLED;

} /* namespace */

#endif
