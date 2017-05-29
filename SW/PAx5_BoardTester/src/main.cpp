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
