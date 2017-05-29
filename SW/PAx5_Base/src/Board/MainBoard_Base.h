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

#ifndef MainBoard_Base_H_
#define MainBoard_Base_H_

#include "defines.h"
#include "MainBoard_Logger.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

#define NVIC_PRIORITY_SPIDMA     1
#define NVIC_PRIORITY_EXTI_0_1   2 // radio, must be higher then NVIC_PRIORITY_SPIDMA !
#define NVIC_PRIORITY_USART      3
#define NVIC_PRIORITY_EXTI_4_15  3 // user switch
#define NVIC_PRIORITY_TIM21      3 // for entropy
#define NVIC_PRIORITY_ADC        3 // for ADC and ADC entropy
#define NVIC_PRIORITY_I2C        3

// -----------------------------------------------------------------------------

/** sysTickCnt is incremented every millisecond by the SysTick_Handler interrupt handler */
extern volatile uint32_t sysTickCnt;

// -----------------------------------------------------------------------------

/**
 * \brief A very basic, blocking, delay function
 * \param ms The number of milliseconds to wait
 * \warning This function needs the SysTick interrupt to be enabled !
 */
void Delay(uint32_t ms);

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------
#endif /* MainBoard_Base_H_ */
