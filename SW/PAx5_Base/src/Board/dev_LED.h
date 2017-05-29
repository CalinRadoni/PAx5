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

#ifndef DEV_LED_H_
#define DEV_LED_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class DEV_LED {
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

protected:
	uint8_t  mode;
	uint32_t timeOn;
	uint32_t timeOff;
	uint8_t  repeatCnt;

	bool ledON;
	uint32_t runTime;
};

extern DEV_LED sLED;

} /* namespace */

#endif
