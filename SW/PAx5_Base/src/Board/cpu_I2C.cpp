/**
 * created 2016.04.23 by Calin Radoni
 */

#include "cpu_I2C.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(6)
#define LOG_CODE_RTimeout   1 // data = slaveAddress
#define LOG_CODE_WTimeout   2 // data = slaveAddress
#define LOG_CODE_IntHandler 3 // data = (uint16_t)intStatus, where intStatus = I2C1->ISR at the entry of the interrupt handler

// -----------------------------------------------------------------------------

CPU_I2C sI2C;

#define TIMEOUT_I2C_INIT ((uint32_t)100) // 100 ms

// -----------------------------------------------------------------------------

CPU_I2C::CPU_I2C() {
	buffLen = 0;
	status = I2CStatus_Disabled;
	transferDone = true;

	myAddress = 0;
	masterMode = true;
	slaveReceiver = true;

	callbackOnReceive = NULL;
	callbackOnRequest = NULL;
}

// -----------------------------------------------------------------------------

void CPU_I2C::Enable(void)
{
	transferDone = true;
	masterMode = true;

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;

	// Timing computed using STM32CubeMX for Standard mode, 100KHz, 100ns rise time, 10ns fall time
	I2C1->TIMINGR = (uint32_t)0x20302E37;
	// Slave address = 0x01, read transfer, 1 byte to receive, autoend - these will change when needed
	I2C1->CR2 = I2C_CR2_AUTOEND | (1<<16) | (0x01<<1);
	// No address, master mode
	I2C1->OAR1 = 0;
	I2C1->OAR2 = 0;
	// No changes to timeout register
	I2C1->TIMEOUTR = 0;
	// Peripheral enable, interrupt enable for: RXNE, TXIS, STOPF and NACKF
	I2C1->CR1 = I2C_CR1_PE | I2C_CR1_RXIE | I2C_CR1_TXIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE;

	NVIC_SetPriority(I2C1_IRQn, NVIC_PRIORITY_I2C);
	NVIC_EnableIRQ(I2C1_IRQn);

	status = I2CStatus_OK;
}

void CPU_I2C::EnableAsSlave(uint8_t mySlaveAddress)
{
	transferDone = true;
	masterMode = false;

	myAddress = mySlaveAddress;

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->CCIPR &= ~RCC_CCIPR_I2C1SEL;

	// Timing computed using STM32CubeMX for Standard mode, 100KHz, 100ns rise time, 10ns fall time
	I2C1->TIMINGR = (uint32_t)0x20302E37;
	// Default value for CR2 register
	I2C1->CR2 = 0;
	// Set only one 7-bit address and enable it
	I2C1->OAR1  = (uint32_t)(myAddress << 1);
	I2C1->OAR1 |= I2C_OAR1_OA1EN;
	I2C1->OAR2 = 0;
	// No changes to timeout register
	I2C1->TIMEOUTR = 0;
	// Peripheral enable, interrupt enable for: RXNE, STOPF, NACKF and ADDRIE
	I2C1->CR1 = I2C_CR1_PE | I2C_CR1_RXIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE | I2C_CR1_ADDRIE;

	NVIC_SetPriority(I2C1_IRQn, NVIC_PRIORITY_I2C);
	NVIC_EnableIRQ(I2C1_IRQn);

	status = I2CStatus_OK;
}

void CPU_I2C::Disable(void)
{
	I2C1->CR1 = I2C1->CR1 & ~(I2C_CR1_PE);
	status |= I2CStatus_Disabled;
}

// -----------------------------------------------------------------------------

void CPU_I2C::Read(uint8_t slaveAddr)
{
	if((status & I2CStatus_Disabled) != 0) Enable();

	status = I2CStatus_OK;
	transferDone = false;
	buffIdx = 0;

	// Set slave address, read transfer, number of bytes to read, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | (buffLen << 16) | (slaveAddr << 1);

	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= TIMEOUT_I2C_INIT){
			status |= I2CStatus_Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_RTimeout, (uint16_t)slaveAddr);
		}
	}
}

void CPU_I2C::Write(uint8_t slaveAddr)
{
	if((status & I2CStatus_Disabled) != 0) Enable();

	status = I2CStatus_OK;
	transferDone = false;
	buffIdx = 0;

	// Set slave address, write transfer, number bytes to send, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | (buffLen << 16) | (slaveAddr << 1);

	I2C1->TXDR = buffer[buffIdx++];
	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= TIMEOUT_I2C_INIT){
			status |= I2CStatus_Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_WTimeout, (uint16_t)slaveAddr);
		}
	}
}

// -----------------------------------------------------------------------------

void CPU_I2C::onReceive(void(*function)(uint8_t))
{
	callbackOnReceive = function;
}
void CPU_I2C::onRequest(void(*function)(void))
{
	callbackOnRequest = function;
}

// -----------------------------------------------------------------------------

void CPU_I2C::HandleI2CInt(void)
{
	uint32_t intStatus;
	uint32_t dataIn;

	intStatus = I2C1->ISR;

	if((intStatus & I2C_ISR_ADDR) == I2C_ISR_ADDR) {
		if(!masterMode){
			// prepare the buffer
			buffIdx = 0;
			// check if is a slave receiver or slave transmitter request
			if((I2C1->ISR & I2C_ISR_DIR) == I2C_ISR_DIR) {
				// slave transmitter
				slaveReceiver = false;
				if(callbackOnRequest != NULL)
					callbackOnRequest();
				// flush I2C_TXDR register and enable I2C TX interrupt
				I2C1->ISR |= I2C_ISR_TXE;
				I2C1->CR1 |= I2C_CR1_TXIE;
			}
			else slaveReceiver = true;
		}
		I2C1->ICR |= I2C_ICR_ADDRCF; // clear address match flag
	}
	else if((intStatus & I2C_ISR_RXNE) == I2C_ISR_RXNE) {
		// read receive register, automatically clear RXNE flag
		dataIn = I2C1->RXDR;
		if(buffIdx < I2CBufferLen){
			buffer[buffIdx] = (uint8_t)(dataIn & 0xFF);
			buffIdx++;
		}
	}
	else if((intStatus & I2C_ISR_NACKF) == I2C_ISR_NACKF){
		status |= I2CStatus_NACK;
		transferDone = true;
		buffLen = buffIdx;
		I2C1->ICR = I2C_ICR_NACKCF; // clear flag
		if(!masterMode){
			// disable TXIE interrupt and flush I2C_TXDR register
			I2C1->CR1 &= ~I2C_CR1_TXIE;
			I2C1->ISR |= I2C_ISR_TXE;
		}
	}
	else if((intStatus & I2C_ISR_TXIS) == I2C_ISR_TXIS){
		// write next byte into I2C_TXDR, automatically clear flag
		if(buffIdx < I2CBufferLen){
			I2C1->TXDR = buffer[buffIdx];
			buffIdx++;
		}
		else I2C1->TXDR = 0;
	}
	else if((intStatus & I2C_ISR_STOPF) == I2C_ISR_STOPF){
		// stop detected (transfer complete)
		transferDone = true;
		buffLen = buffIdx;
		I2C1->ICR = I2C_ICR_STOPCF; // clear flag
		if(!masterMode){
			if(slaveReceiver){
				if(callbackOnReceive != NULL)
					callbackOnReceive(buffLen);
			}
			else {
				// disable TXIE interrupt and flush I2C_TXDR register
				I2C1->CR1 &= ~I2C_CR1_TXIE;
				I2C1->ISR |= I2C_ISR_TXE;
			}
		}
	}
	else {
		status |= (I2CStatus_IntfErr | I2CStatus_Disabled);
		hwLogger.AddEntry(FileID, LOG_CODE_IntHandler, (uint16_t)intStatus);
		NVIC_DisableIRQ(I2C1_IRQn);
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void I2C1_IRQHandler(void)
{
	PAx5::sI2C.HandleI2CInt();
}

// -----------------------------------------------------------------------------

