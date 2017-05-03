/**
 * created 2016.02.20 by Calin Radoni
 */

#include "cpu_USART1.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(7)
#define LOG_CODE_CfgTimeout  1 // data unused
#define LOG_CODE_UB_Overflow 2 // data unused
#define LOG_CODE_IntHandler  3 // data = (uint16_t)USART1->ISR

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
}

bool CPU_USART1::Configure(void)
{
	// Enable the peripheral clock for USART1
	// USART1 clock is PCLK2 (APB2 peripheral clocks)
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	// Baud rate 115200
	USART1->BRR = 0x116;
	// 8N1, Oversampling by 16, Enable Tx, Rx and UART
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
