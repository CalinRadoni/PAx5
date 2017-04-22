/**
 * created 2016.11.14 by Calin Radoni
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
