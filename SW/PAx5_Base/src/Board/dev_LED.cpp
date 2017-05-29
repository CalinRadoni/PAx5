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

#include "dev_LED.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

#define MODE_Manual       0
#define MODE_ON           1
#define MODE_Blink        2

#define TURN_LED_ON()  { GPIOA->BSRR = GPIO_BSRR_BR_15; }
#define TURN_LED_OFF() { GPIOA->BSRR = GPIO_BSRR_BS_15; }

// -----------------------------------------------------------------------------

DEV_LED sLED;

// -----------------------------------------------------------------------------

DEV_LED::DEV_LED()
{
	mode = MODE_Manual;
}

// -----------------------------------------------------------------------------

void DEV_LED::On(void)
{
	mode = MODE_Manual;
	TURN_LED_ON();
}

void DEV_LED::Off(void)
{
	mode = MODE_Manual;
	TURN_LED_OFF();
}

void DEV_LED::Blink(uint32_t ms){
	mode = MODE_Manual;

	TURN_LED_ON();
	Delay(ms);
	TURN_LED_OFF();
	Delay(ms);
}

// -----------------------------------------------------------------------------

void DEV_LED::ModeOn(uint32_t ms)
{
	mode = MODE_ON;
	timeOn = ms;
	TURN_LED_ON();
}

void DEV_LED::ModeBlink(uint32_t ms)
{
	if(ms == 0){
		mode = MODE_Manual;
		TURN_LED_OFF();
		return;
	}

	mode    = MODE_Blink;
	timeOn  = ms;
	timeOff = ms;
	repeatCnt = 1;
	TURN_LED_ON();
}

void DEV_LED::ModeBlink(uint32_t msOn, uint32_t msOff, uint8_t cnt)
{
	if(((msOn == 0) && (msOff == 0)) || (cnt == 0)){
		mode = MODE_Manual;
		TURN_LED_OFF();
		return;
	}

	mode    = MODE_Blink;
	timeOn  = msOn;
	timeOff = msOff;
	repeatCnt = cnt;
	if(timeOn > 0) { TURN_LED_ON();  ledON = true;  runTime = timeOn;  }
	else           { TURN_LED_OFF(); ledON = false; runTime = timeOff; }
}

bool DEV_LED::Pulse(uint32_t ms)
{
	if(mode == MODE_Manual) return false;

	if(mode == MODE_ON){
		if(ms < timeOn) timeOn -= ms;
		else{
			mode = MODE_Manual;
			TURN_LED_OFF();
			return false;
		}
		return true;
	}

	// MODE_Blink

	if(ms < runTime){
		runTime -= ms;
		return true;
	}

	if(ledON){
		TURN_LED_OFF();
		runTime = timeOff;
	}
	else{
		// cycle done

		repeatCnt--;
		if(repeatCnt == 0){
			mode = MODE_Manual;
			TURN_LED_OFF();
			return false;
		}

		TURN_LED_ON();
		runTime = timeOn;
	}
	ledON = !ledON;
	return true;
}

// -----------------------------------------------------------------------------
} /* namespace */
