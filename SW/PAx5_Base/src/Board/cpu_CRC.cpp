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

#include "cpu_CRC.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_CRC sCRC;

// -----------------------------------------------------------------------------

CPU_CRC::CPU_CRC()
{
}

CPU_CRC::~CPU_CRC()
{
	Disable();
}

// -----------------------------------------------------------------------------

void CPU_CRC::Enable(void)
{
	// enable CRC clock ...
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	// ... and set default values
	CRC->INIT  = 0xFFFFFFFF;
	CRC->CR    = CRC_CR_REV_OUT | CRC_CR_REV_IN;
	CRC->POL   = 0x04C11DB7;
	CRC->CR   |= CRC_CR_RESET;
}

void CPU_CRC::Disable(void)
{
	// disable CRC clock
	RCC->AHBENR &= ~(RCC_AHBENR_CRCEN);
}

// -----------------------------------------------------------------------------

void CPU_CRC::AddData32(uint32_t data)
{
	CRC->DR = __REV(data);
}

void CPU_CRC::AddData8(uint8_t data)
{
	((__IO uint8_t*)&CRC->DR)[0] = data;
}

uint32_t CPU_CRC::GetData(void)
{
	return CRC->DR ^ 0xFFFFFFFF;
}

// -----------------------------------------------------------------------------
} /* namespace PAx5 */
