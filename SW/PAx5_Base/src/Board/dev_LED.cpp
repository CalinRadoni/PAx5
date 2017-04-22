/**
 * created 2016.06.16 by Calin Radoni
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
