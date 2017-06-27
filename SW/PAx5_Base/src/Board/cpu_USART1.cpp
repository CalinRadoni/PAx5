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

#include "cpu_USART1.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(7)
#define LOG_CODE_CfgTimeout  1 // data unused
#define LOG_CODE_UB_Overflow 2 // data unused
#define LOG_CODE_IntHandler  3 // data = (uint16_t)USART1->ISR
#define LOG_CODE_BaudRate    4 // 0: requested 0, 1: too small, 2: too big, 3: error too big >= 3%

// -----------------------------------------------------------------------------

CPU_USART1 sUSART;

// -----------------------------------------------------------------------------

const uint32_t Timeout_USART_Init = 1000U; // 1 s

// -----------------------------------------------------------------------------

CPU_USART1::CPU_USART1() {
	Initialize();
}

CPU_USART1::~CPU_USART1() {
}

// -----------------------------------------------------------------------------

void CPU_USART1::Initialize(void)
{
	buffRxIdx = 0;
	dataReceived = false;
	dataOverflow = false;

	buffTxIdx = 0;
	buffTxLen = 0;
	dataSent = false;

	intfError = false;

	USARTDivider = 0;
}

bool CPU_USART1::ComputeUSARTDivider(uint32_t fck, uint32_t baudRate)
{
	if(baudRate == 0){
		hwLogger.AddEntry(FileID, LOG_CODE_BaudRate, 0);
		USARTDivider = 0;
		return false;
	}

	USARTDivider = fck;
	USARTDivider *= 100U;
	USARTDivider /= baudRate;
	USARTDivider += 50U;
	USARTDivider /= 100U;

	/**
	 * RM0377, 24.5.4 USART baud rate generation: USARTDIV must be greater than or equal to 0d16 !
	 */
	if(USARTDivider < 0x10){
		hwLogger.AddEntry(FileID, LOG_CODE_BaudRate, 1);
		USARTDivider = 0;
		return false;
	}

	/**
	 * BRR is 16 bits in length
	 */
	if(USARTDivider > 0x00FFFFU){
		hwLogger.AddEntry(FileID, LOG_CODE_BaudRate, 2);
		USARTDivider = 0;
		return false;
	}

	/**
	 * Compute the value of baud rate for computed USARTDivider
	 */
	uint32_t error = fck;
	error *= 100U;
	error /= USARTDivider;
	error += 50U;
	error /= 100U;


	/**
	 * RM 0377, 24.5.5 Tolerance of the USART receiver to clock deviation: Error should be under 3%.
	 */
	if(error > baudRate) error -= baudRate;
	else                 error = baudRate - error;
	error *= 10000U;
	error /= baudRate;
	if(error >= 300){
		hwLogger.AddEntry(FileID, LOG_CODE_BaudRate, 3);
		USARTDivider = 0;
		return false;
	}

	return true;
}

bool CPU_USART1::Configure(void)
{
	if(USARTDivider == 0) return false;

	// Enable the peripheral clock for USART1
	// USART1 clock is PCLK2 (APB2 peripheral clocks)
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Set the baud rate
	USART1->BRR = USARTDivider;
	// 8N1, Oversampling by 16, Enable Tx, Rx and USART
	USART1->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	// Wait for idle frame Transmission ...
	uint32_t timeStart = sysTickCnt;
	while((USART1->ISR & USART_ISR_TC) != USART_ISR_TC){
		if((sysTickCnt - timeStart) >= Timeout_USART_Init){
			hwLogger.AddEntry(FileID, LOG_CODE_CfgTimeout, 0);
			return false;
		}
	}
	// ... clear TC flag ...
	USART1->ICR = USART_ICR_TCCF;
	// ... and enable interrupts
	USART1->CR1 |= USART_CR1_TCIE | USART_CR1_RXNEIE;

	// Set priority for USART1_IRQn and enable USART1_IRQn
	NVIC_SetPriority(USART1_IRQn, NVIC_PRIORITY_USART);
	NVIC_EnableIRQ(USART1_IRQn);

	intfError = false;

	return true;
}

// -----------------------------------------------------------------------------

void CPU_USART1::InitTxData(void)
{
	buffTxLen = 0;
}

void CPU_USART1::AddTxData(uint8_t data)
{
	if(buffTxLen >= U1_TX_BUFF_LEN){
		buffTxLen = U1_TX_BUFF_LEN;

		SendTxData();
		while(!DataIsSent()) { __NOP(); } // wait;
		buffTxLen = 0;
	}

	bufferTX[buffTxLen++] = data;
}

uint8_t CPU_USART1::TxDataAvailableSpace(void)
{
	return (U1_TX_BUFF_LEN - buffTxLen);
}

void CPU_USART1::SendTxData(void)
{
	if(intfError) return;

	buffTxIdx = 0;
	if(buffTxLen == 0){
		dataSent = true;
		return;
	}
	dataSent = false;
	USART1->TDR = bufferTX[buffTxIdx++];
}

void CPU_USART1::SendTxDataAndWait(void)
{
	if(intfError) return;

	buffTxIdx = 0;
	if(buffTxLen == 0){
		dataSent = true;
		return;
	}
	dataSent = false;
	USART1->TDR = bufferTX[buffTxIdx++];

	while(!DataIsSent()) { __NOP(); }
	buffTxLen = 0;
}

bool CPU_USART1::DataIsSent(void)
{
	return (dataSent || intfError);
}

// -----------------------------------------------------------------------------

void CPU_USART1::ClearRXBuffer(void)
{
	uint32_t pMask;

	pMask = __get_PRIMASK();
	__disable_irq();

	sUSART.buffRxIdx = 0;
	sUSART.dataReceived = false;
	sUSART.dataOverflow = false;

	if(pMask == 0)
		__enable_irq();
}

// -----------------------------------------------------------------------------

void CPU_USART1::HandleInterrupt(void)
{
	uint8_t ch;

	// TODO USART: Handle USART overflow and other events

	if((USART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		ch = (uint8_t)(USART1->RDR); // Receive data, flag cleared automatically
		if(buffRxIdx < U1_RX_BUFF_LEN) bufferRX[buffRxIdx++] = ch;
		else {
			dataOverflow = true;
			hwLogger.AddEntry(FileID, LOG_CODE_UB_Overflow, 0);
		}
		dataReceived = true;
	}
	else{
		if((USART1->ISR & USART_ISR_TC) == USART_ISR_TC){
			if(buffTxIdx < buffTxLen){
				USART1->TDR = bufferTX[buffTxIdx++];	// TC flag automatically cleared
			}
			else{
				USART1->ICR = USART_ICR_TCCF;			// Clear transfer complete flag
				dataSent = true;
				buffTxLen = 0;
			}
		}
		else{
			intfError = true;
			hwLogger.AddEntry(FileID, LOG_CODE_IntHandler, (uint16_t)USART1->ISR);
			NVIC_DisableIRQ(USART1_IRQn); // Disable USART1_IRQn
		}
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void USART1_IRQHandler(void)
{
	PAx5::sUSART.HandleInterrupt();
}

// -----------------------------------------------------------------------------
