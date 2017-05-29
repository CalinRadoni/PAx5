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

#include "cpu_DMA.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_DMA sDMA;

// -----------------------------------------------------------------------------

CPU_DMA::CPU_DMA() { /* */ }

// -----------------------------------------------------------------------------

void CPU_DMA::M2M_Initialize(void)
{
	// Enable the peripheral clock for DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	/** DMA1 channel 1 settings:
	 * - memory to memory mode
	 * - channel priority level = very high
	 * - memory size = 8 bits
	 * - peripheral size = 8 bits
	 * - memory increment mode enabled
	 * - peripheral increment mode enabled
	 * - circular mode disabled
	 * - direction = 0 = read from peripheral
	 * - no interrupts
	 * - not enabled
	 */
	DMA1_Channel1->CCR = DMA_CCR_MEM2MEM | DMA_CCR_PL | DMA_CCR_MINC | DMA_CCR_PINC;

	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S; // reset request selection for DMA1 channel 1

	DMA1->IFCR = DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1; // clear flags
}

void CPU_DMA::M2M_Copy(uint32_t srcAddr, uint32_t dstAddr, uint32_t dataLen)
{
	DMA1_Channel1->CCR &= ~DMA_CCR_EN; // disable

	DMA1->IFCR = DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1; // clear flags

	DMA1_Channel1->CNDTR = dataLen;
	DMA1_Channel1->CPAR  = srcAddr;
	DMA1_Channel1->CMAR  = dstAddr;

	DMA1_Channel1->CCR |= DMA_CCR_EN; // enable, transfer starts
}

void CPU_DMA::M2M_CleanUp(void)
{
	DMA1_Channel1->CCR &= ~DMA_CCR_EN; // disable

	DMA1_Channel1->CCR   = 0U;
	DMA1_Channel1->CNDTR = 0U;
	DMA1_Channel1->CPAR  = 0U;
	DMA1_Channel1->CMAR  = 0U;

	DMA1->IFCR = DMA_IFCR_CGIF1 | DMA_IFCR_CTCIF1 | DMA_IFCR_CHTIF1 | DMA_IFCR_CTEIF1; // clear flags

	DMA1_CSELR->CSELR &= ~DMA_CSELR_C1S; // reset request selection for DMA1 channel 1
}

uint8_t CPU_DMA::M2M_TransferStatus(void)
{
	uint8_t res;

	res = DMA1->ISR & (DMA_ISR_GIF1 | DMA_ISR_TCIF1 | DMA_ISR_TEIF1);
	if((res & DMA_ISR_TEIF1) != 0) return 2;
	if((res & DMA_ISR_TCIF1) != 0) return 1;
	return 0;
}

// -----------------------------------------------------------------------------
} /* namespace */
