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

#ifndef cpu_CLOCKS_H_
#define cpu_CLOCKS_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_Clocks {
public:
	CPU_Clocks();
	virtual ~CPU_Clocks();

	/**
	 * RCC_CFGR HPRE[3:0] is the AHB prescaler.
	 * HCLK = SYSCLK / AHB prescaler value
	 * 0xxx: SYSCLK not divided
	 * 1000: SYSCLK divided by 2
	 * 1001: SYSCLK divided by 4
	 * 1010: SYSCLK divided by 8
	 * 1011: SYSCLK divided by 16
	 * 1100: SYSCLK divided by 64
	 * 1101: SYSCLK divided by 128
	 * 1110: SYSCLK divided by 256
	 * 1111: SYSCLK divided by 512
	 */
	uint8_t HPRE;

	/**
	 * RCC_CFGR PPRE1[2:0] is APB low-speed prescaler, APB1
	 * PCLK1 = HCLK / APB1 prescaler value
	 *
	 * RCC_CFGR PPRE2[2:0] is APB high-speed prescaler, APB2
	 * PCLK2 = HCLK / APB2 prescaler value
	 *
	 * 0xx: HCLK not divided
	 * 100: HCLK divided by 2
	 * 101: HCLK divided by 4
	 * 110: HCLK divided by 8
	 * 111: HCLK divided by 16
	 */
	uint8_t APB1, APB2;

	// SYSCLK
	volatile uint32_t clockSYS;

	/**
	 * Clock values:
	 * HCLK = SYSCLK / HPREvalue
	 *
	 * PCLK1 = HCLK / APB1value
	 * PCLK2 = HCLK / APB2value
	 *
	 * I2C1 clock   = {PCLK1, HSI 16, SYSCLK}
	 * USART1 clock = {PCLK2, SYSCLK, HSI 16, LSE }
	 * SPI clock    = PCLK2
	 * ADC clock    = HSI 16
	 */
	//
	volatile uint32_t clockHCLK, clockI2C1, clockUSART1, clockSPI, clockADC;

	/**
	 * \brief Update the values of clocks based on register values
	 *
	 * The variable #clockSYS should be set before calling this function
	 *
	 * Set the value of:
	 * - #HPRE, #APB1 and #APB2
	 * - #clockHCLK
	 * - #clockI2C1
	 * - #clockUSART1
	 * - #clockSPI
	 * - #clockADC
	 * based of #clockSYS and RCC_xxx registers
	 */
	void UpdateClockValues(void);

	/**
	 * Created to be called after startup, sets HPRE, APB1, APB2, I2C1 and USART1 clock sources
	 */
	void Startup_SetValues(void);
};

} /* namespace */

#endif
