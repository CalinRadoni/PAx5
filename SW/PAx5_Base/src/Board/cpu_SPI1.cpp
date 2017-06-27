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

#include "cpu_SPI1.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(5)
#define LOG_CODE_SCW        1 // data = (cmd << 8 | spiSlave)
#define LOG_CODE_SCPW       2 // data = (cmd << 8 | spiSlave)
#define LOG_CODE_SBW        3 // data = spiSlave
#define LOG_CODE_IntHandler 4 // data = ((DMA1->ISR >> 4) << 8 | spiSlave)

// -----------------------------------------------------------------------------

CPU_SPI1 sSPI;

// -----------------------------------------------------------------------------

CPU_SPI1::CPU_SPI1() {
	Initialize();
	defaultSPISettings = false;
}

CPU_SPI1::~CPU_SPI1() {
}

// -----------------------------------------------------------------------------

void CPU_SPI1::Initialize(void)
{
	transferDone = true;
	intfError    = false;
	spiSlave     = SPISlave_None;
	readyToSend  = false;

	clockDivider = 2;
}

void CPU_SPI1::Configure(void)
{
	// Enable RX DMA
	SPI1->CR2 = SPI_CR2_RXDMAEN;

	// Enable the peripheral clock for DMA1
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	// DMA1 Channel 2 <- SPI1 RX (C2S = 0001b)
	DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C2S) | 0x00000010;
	// Peripheral address
	DMA1_Channel2->CPAR = (uint32_t)&(SPI1->DR);
	// Memory address
	DMA1_Channel2->CMAR = (uint32_t)smallBuffer;
	// Transfer size
	DMA1_Channel2->CNDTR = 1;
	// Memory increment, Peripheral->Memory, Int: TEIE + TCIE, Channel Enabled
	DMA1_Channel2->CCR |= DMA_CCR_MINC | DMA_CCR_TEIE | DMA_CCR_TCIE;

	// DMA1 Channel3 <- SPI1 TX (C3S = 0001b)
	DMA1_CSELR->CSELR = (DMA1_CSELR->CSELR & ~DMA_CSELR_C3S) | 0x00000100;
	// Peripheral address
	DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR);
	// Memory address
	DMA1_Channel3->CMAR = (uint32_t)smallBuffer;
	// Transfer size
	DMA1_Channel2->CNDTR = 0;
	// Memory increment, Memory->Peripheral
	DMA1_Channel3->CCR |= DMA_CCR_MINC | DMA_CCR_DIR;

	// Set priority and enable DMA1 interrupt
	NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);
	NVIC_SetPriority(DMA1_Channel2_3_IRQn, NVIC_PRIORITY_SPIDMA);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	// Enable the peripheral clock for SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Master, CPOL=0, CPHA=0, DFF=0 (8 bit data frame)
	// SSM = 1 (Software NSS Management), SSI = 1 (SlaveSelectInternal, if 0 -> master conflict)
	// Baud rate control = (100) = fPCLK / 32 -> 1 MHz (SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_2;)
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
	if(clockDivider & 0x04) SPI1->CR1 |= SPI_CR1_BR_2;
	if(clockDivider & 0x02) SPI1->CR1 |= SPI_CR1_BR_1;
	if(clockDivider & 0x01) SPI1->CR1 |= SPI_CR1_BR_0;

	// Slave Select Output DISABLED, No Interrupts, DMA Both
	SPI1->CR2 = SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;
	// Enable SPI1
	SPI1->CR1 |= SPI_CR1_SPE;

	intfError = false;
}

void CPU_SPI1::SendCommandAndWait(uint8_t cmd, uint8_t slaveIn)
{
	while(!transferDone){ __NOP(); }

	if(intfError)
		Configure();
	transferDone = false;

	spiSlave = slaveIn;
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BR_4;	// select memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BR_0;	// select radio
			break;
		default:
			break;
	}

	smallBuffer[0] = cmd;

	// prepare RX and enable
	DMA1_Channel2->CMAR  = (uint32_t)smallBuffer;
	DMA1_Channel2->CNDTR = 1;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	// prepare TX and enable -> start transmission
	DMA1_Channel3->CMAR  = (uint32_t)smallBuffer;
	DMA1_Channel3->CNDTR = 1;
	DMA1_Channel3->CCR |= DMA_CCR_EN;

	while(!transferDone){ __NOP(); }

	if(intfError){
		uint16_t logData;
		logData = (uint16_t)cmd;
		logData = logData << 8;
		logData = logData | spiSlave;
		hwLogger.AddEntry(FileID, LOG_CODE_SCW, logData);
	}
}

uint8_t CPU_SPI1::SendCmdPlusAndWait(uint8_t cmd, uint8_t data, uint8_t slaveIn)
{
	while(!transferDone){ __NOP(); }

	if(intfError)
		Configure();
	transferDone = false;

	spiSlave = slaveIn;
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BR_4;	// select memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BR_0;	// select radio
			break;
		default:
			break;
	}

	smallBuffer[0] = cmd;
	smallBuffer[1] = data;

	// prepare RX and enable
	DMA1_Channel2->CMAR  = (uint32_t)smallBuffer;
	DMA1_Channel2->CNDTR = 2;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	// prepare TX and enable -> start transmission
	DMA1_Channel3->CMAR  = (uint32_t)smallBuffer;
	DMA1_Channel3->CNDTR = 2;
	DMA1_Channel3->CCR |= DMA_CCR_EN;

	while(!transferDone){ __NOP(); }

	if(intfError){
		uint16_t logData;
		logData = (uint16_t)cmd;
		logData = logData << 8;
		logData = logData | spiSlave;
		hwLogger.AddEntry(FileID, LOG_CODE_SCPW, logData);
	}

	return smallBuffer[1];
}

void CPU_SPI1::SendBufferAndWait(volatile uint8_t* inputBuffer, uint16_t inputBufLen, uint8_t slaveIn)
{
	while(!transferDone){ __NOP(); }

	if(intfError)
		Configure();

	if(inputBufLen == 0){
		transferDone = true;
		return;
	}

	transferDone = false;

	spiSlave = slaveIn;
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BR_4;	// select memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BR_0;	// select radio
			break;
		default:
			break;
	}

	// prepare RX and enable
	DMA1_Channel2->CMAR  = (uint32_t)inputBuffer;
	DMA1_Channel2->CNDTR = inputBufLen;
	DMA1_Channel2->CCR |= DMA_CCR_EN;

	// prepare TX and enable -> start transmission
	DMA1_Channel3->CMAR  = (uint32_t)inputBuffer;
	DMA1_Channel3->CNDTR = inputBufLen;
	DMA1_Channel3->CCR |= DMA_CCR_EN;

	while(!transferDone){ __NOP(); }

	if(intfError)
		hwLogger.AddEntry(FileID, LOG_CODE_SBW, (uint16_t)spiSlave);
}

// -----------------------------------------------------------------------------

bool CPU_SPI1::ConfigureForRAM(void)
{
	if(!transferDone) return false;
	if(!defaultSPISettings) return false;

	// Disable DMA1 Channel 2 and 3 interrupt
	NVIC_DisableIRQ(DMA1_Channel2_3_IRQn);

	// Enable the peripheral clock for SPI1
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	// Master, CPOL=0, CPHA=0, DFF=0 (8 bit data frame)
	// SSM = 1 (Software NSS Management), SSI = 1 (SlaveSelectInternal, if 0 -> master conflict)
	// Baud rate control = (100) = fPCLK / 32 -> 1 MHz (SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_2;)
	SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
	if(clockDivider & 0x04) SPI1->CR1 |= SPI_CR1_BR_2;
	if(clockDivider & 0x02) SPI1->CR1 |= SPI_CR1_BR_1;
	if(clockDivider & 0x01) SPI1->CR1 |= SPI_CR1_BR_0;

	// Slave Select Output DISABLED, No Interrupts
	SPI1->CR2 = 0;
	// Enable SPI1
	SPI1->CR1 |= SPI_CR1_SPE;

	intfError = false;

	return true;
}
__RAMFUNC uint8_t CPU_SPI1::RAM_SendCmdPlusAndWait(uint8_t cmd, uint8_t data, uint8_t slaveIn)
{
	volatile uint8_t rxData;

	// wait for TXE
	while((SPI1->SR & SPI_SR_TXE) == 0) { __NOP(); }

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	// select slave
	spiSlave = slaveIn;
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BR_4;	// select memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BR_0;	// select radio
			break;
		default:
			break;
	}

	// begin transmission ...
	*(uint8_t *)&(SPI1->DR) = cmd;
	// ... wait for transmision to begin ...
	while((SPI1->SR & SPI_SR_BSY) == 0) { __NOP(); }
	// ... wait for TXE ...
	while((SPI1->SR & SPI_SR_TXE) == 0) { __NOP(); }
	// ... write next byte ...
	*(uint8_t *)&(SPI1->DR) = data;
	// ... wait for RXNE
	while((SPI1->SR & SPI_SR_RXNE) == 0) { __NOP(); }
	// ... and read received data
	rxData = (uint8_t)SPI1->DR;
	// ... wait for RXNE
	while((SPI1->SR & SPI_SR_RXNE) == 0) { __NOP(); }
	// ... and read received data
	rxData = (uint8_t)SPI1->DR;

	// deselect slave
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BS_4; // deselect memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BS_0; // deselect radio
			break;
		default:
			break;
	}

	if(pMask == 0) __enable_irq();

	return rxData;
}
__RAMFUNC void CPU_SPI1::RAM_SendBufferAndWait(volatile uint8_t* inputBuffer, uint16_t inputBufLen, uint8_t slaveIn)
{
	uint8_t idxRx, idxTx;

	if(inputBufLen == 0) return;

	idxRx = 0;
	idxTx = 0;

	// wait for TXE
	while((SPI1->SR & SPI_SR_TXE) == 0) { __NOP(); }

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	// select slave
	spiSlave = slaveIn;
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BR_4;	// select memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BR_0;	// select radio
			break;
		default:
			break;
	}

	// begin transmission ...
	*(uint8_t *)&(SPI1->DR) = inputBuffer[idxTx];
	idxTx++;
	// ... wait for transmision to begin
	while((SPI1->SR & SPI_SR_BSY) == 0) { __NOP(); }
	// ... start the TX/RX loop
	while(idxRx < inputBufLen){
		if(idxTx < inputBufLen){
			while((SPI1->SR & SPI_SR_TXE) == 0) { __NOP(); }
			*(uint8_t *)&(SPI1->DR) = inputBuffer[idxTx];
			idxTx++;
		}
		while((SPI1->SR & SPI_SR_RXNE) == 0) { __NOP(); }
		inputBuffer[idxRx] = (uint8_t)SPI1->DR;
		idxRx++;
	}

	// deselect slave
	switch(spiSlave){
		case SPISlave_Memory:
			GPIOA->BSRR = GPIO_BSRR_BS_4; // deselect memory
			break;
		case SPISlave_Radio:
			GPIOB->BSRR = GPIO_BSRR_BS_0; // deselect radio
			break;
		default:
			break;
	}

	if(pMask == 0) __enable_irq();
}

// -----------------------------------------------------------------------------

void CPU_SPI1::HandleDMAInt(void)
{
	if((DMA1->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2){
		DMA1->IFCR |= DMA_IFCR_CTCIF2; // Clear Transfer Complete flag

		while((SPI1->SR & SPI_SR_BSY) == SPI_SR_BSY){} // wait for SPI to finish

		switch(spiSlave){
			case SPISlave_Memory:
				GPIOA->BSRR = GPIO_BSRR_BS_4; // deselect memory
				break;
			case SPISlave_Radio:
				GPIOB->BSRR = GPIO_BSRR_BS_0; // deselect radio
				break;
			default:
				break;
		}

		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		DMA1_Channel2->CCR &= ~DMA_CCR_EN;
		transferDone = true;
	}
	else {
		intfError = true;
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		DMA1_Channel2->CCR &= ~DMA_CCR_EN;

		uint16_t logData;
		logData = (uint16_t)(DMA1->ISR >> 4);
		logData = logData << 8;
		logData = logData | spiSlave;
		hwLogger.AddEntry(FileID, LOG_CODE_IntHandler, logData);

		NVIC_DisableIRQ(DMA1_Channel2_3_IRQn); // Disable DMA1_Channel2_3_IRQn
		transferDone = true;
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void DMA1_Channel2_3_IRQHandler(void)
{
	PAx5::sSPI.HandleDMAInt();
}

// -----------------------------------------------------------------------------
