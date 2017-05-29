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

#include "MainBoard_Base.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

volatile uint32_t sysTickCnt = 0;

// -----------------------------------------------------------------------------

void Delay(uint32_t msIn){
	uint32_t timeStart = sysTickCnt;

	while((sysTickCnt - timeStart) < msIn)
	{
		__NOP();
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------
// Cortex-M0 Plus Processor Exceptions Handlers
// -----------------------------------------------------------------------------

extern "C" void NMI_Handler(void)
{
}

extern "C" void HardFault_Handler(void)
{
#if 0
	// enable this to debug hard fault exception
	__ASM volatile("BKPT #01");
#endif

	NVIC_SystemReset();
}

extern "C" void SVC_Handler(void)
{
}

extern "C" void PendSV_Handler(void)
{
}

extern "C" void SysTick_Handler(void)
{
	PAx5::sysTickCnt++;
}

// -----------------------------------------------------------------------------
