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
	uint32_t boardCapabilities;

	boardCapabilities = PAx5_BOARD_HW_USART | PAx5_BOARD_HW_I2C | PAx5_BOARD_HW_RFM69HW | PAx5_BOARD_HW_ExtFLASH;

	PAx5::board.InitializeBoard(boardCapabilities);

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
