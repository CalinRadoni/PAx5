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

#include <cpu_Clocks.h>

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_Clocks::CPU_Clocks()
{
}

CPU_Clocks::~CPU_Clocks()
{
}

// -----------------------------------------------------------------------------

void CPU_Clocks::UpdateClockValues(void)
{
	uint32_t val = RCC->CFGR;
	val &= RCC_CFGR_HPRE;
	val = val >> RCC_CFGR_HPRE_Pos;
	HPRE = (uint8_t)(val & 0x00FFU);

	val = RCC->CFGR & RCC_CFGR_PPRE1;
	val = val >> RCC_CFGR_PPRE1_Pos;
	APB1 = (uint8_t)(val & 0x00FFU);

	val = RCC->CFGR & RCC_CFGR_PPRE2;
	val = val >> RCC_CFGR_PPRE2_Pos;
	APB2 = (uint8_t)(val & 0x00FFU);

	switch(HPRE){
		case  8: val =   2; break;
		case  9: val =   4; break;
		case 10: val =   8; break;
		case 11: val =  16; break;
		case 12: val =  64; break;
		case 13: val = 128; break;
		case 14: val = 256; break;
		case 15: val = 512; break;
		default: val =   1; break;
	}
	clockHCLK = clockSYS / val;

	switch(APB1){
		case 4:  val =  2; break;
		case 5:  val =  4; break;
		case 6:  val =  8; break;
		case 7:  val = 16; break;
		default: val =  1; break;
	}
	uint32_t PCLK1 = clockHCLK / val;

	switch(APB2){
		case 4:  val =  2; break;
		case 5:  val =  4; break;
		case 6:  val =  8; break;
		case 7:  val = 16; break;
		default: val =  1; break;
	}
	uint32_t PCLK2 = clockHCLK / val;

	uint32_t clockHSI16 = 16000000;
	if((RCC->CR & RCC_CR_HSIDIVEN) != 0)
		clockHSI16 /= 4;

	val = RCC->CCIPR & RCC_CCIPR_I2C1SEL;
	val = val >> RCC_CCIPR_I2C1SEL_Pos;
	switch(val){
		case 0:  clockI2C1 = PCLK1;      break;
		case 1:  clockI2C1 = clockSYS;   break;
		case 2:  clockI2C1 = clockHSI16; break;
		default: clockI2C1 = 0;          break;
	}

	val = RCC->CCIPR & RCC_CCIPR_USART1SEL;
	val = val >> RCC_CCIPR_USART1SEL_Pos;
	switch(val){
		case 0:  clockUSART1 = PCLK2;      break;
		case 1:  clockUSART1 = clockSYS;   break;
		case 2:  clockUSART1 = clockHSI16; break;
		default: clockUSART1 = 0;          break; // Actually here should be LSE
	}

	clockSPI = PCLK2;

	clockADC = clockHSI16;
}

// -----------------------------------------------------------------------------

void CPU_Clocks::Startup_SetValues(void)
{
	uint32_t val;


	val = RCC->CFGR;

	val &= ~(RCC_CFGR_HPRE);
	val |= RCC_CFGR_HPRE_DIV2;

	val &= ~(RCC_CFGR_PPRE1);
	val |= RCC_CFGR_PPRE1_DIV1;

	val &= ~(RCC_CFGR_PPRE2);
	val |= RCC_CFGR_PPRE2_DIV1;

	RCC->CFGR = val;

	val = RCC->CCIPR;

	val &= ~(RCC_CCIPR_I2C1SEL);
	val |= RCC_CCIPR_I2C1SEL_1;

	val &= ~(RCC_CCIPR_USART1SEL);
	val |= RCC_CCIPR_USART1SEL_1;

	RCC->CCIPR = val;
}

// -----------------------------------------------------------------------------
} /* namespace */
