/**
 * created 2016.11.01 by Calin Radoni
 */

#include "MainBoard.h"
#include "dev_LED.h"
#include "BoardTester.h"

// -----------------------------------------------------------------------------

PAx5::BoardTester tester;

int main(void)
{
	PAx5::BoardDefinion boardDefinition;
	boardDefinition.SetByType(PAx5::BoardDefinion::BoardType::PAx5_BaseBoard);

	if(PAx5::board.InitializeBoard(boardDefinition) != PAx5::MainBoard::Error::OK){
		PAx5::board.BlinkError();
		while(1){ /* infinite loop */ }
	}

	PAx5::sLED.Blink(300);
	PAx5::sLED.Blink(300);
	PAx5::sLED.Blink(300);

	while(1){
		tester.InteractiveTest();
	}

#if 0
	while(1){
		if(exti8Rised){
			LED_On();
			Delay(500);
			LED_Off();
			exti8Rised = false;
		}
		Delay(10);
	}
#endif
}

// -----------------------------------------------------------------------------
