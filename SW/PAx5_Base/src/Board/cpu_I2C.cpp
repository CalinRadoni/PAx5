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

#include "cpu_I2C.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(6)
#define LOG_CODE_RTimeout   1 // data = slaveAddress
#define LOG_CODE_WTimeout   2 // data = slaveAddress
#define LOG_CODE_CRTimeout  3 // data = slaveAddress
#define LOG_CODE_CWTimeout  4 // data = slaveAddress
#define LOG_CODE_IntHandler 9 // data = (uint16_t)intStatus, where intStatus = I2C1->ISR at the entry of the interrupt handler

// -----------------------------------------------------------------------------

CPU_I2C sI2C;

const uint32_t I2C_TransferTimeout = 1000; // 1 second

// -----------------------------------------------------------------------------

CPU_I2C::CPU_I2C() {
	buffLen = 0;
	status = Status::Disabled;
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

	status = Status::OK;
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

	status = Status::OK;
}

void CPU_I2C::Disable(void)
{
	I2C1->CR1 = I2C1->CR1 & ~(I2C_CR1_PE);
	status = Status::Disabled;
}

// -----------------------------------------------------------------------------

CPU_I2C::Status CPU_I2C::Test(uint8_t slaveAddr)
{
	if((status == Status::Disabled) || (status == Status::IntfErr)) Enable();

	status = Status::OK;
	transferDone = false;
	buffIdx = 1;
	buffLen = 0;

	// Set slave address, write transfer, 0 bytes to send, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | (slaveAddr << 1);

	I2C1->TXDR = 0;
	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= I2C_TransferTimeout){
			status = Status::Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_WTimeout, (uint16_t)slaveAddr);
		}
	}

	return status;
}

// -----------------------------------------------------------------------------

CPU_I2C::Status CPU_I2C::Read(uint8_t slaveAddr, uint8_t cnt)
{
	if((status == Status::Disabled) || (status == Status::IntfErr)) Enable();

	status = Status::OK;
	transferDone = false;
	buffIdx = 0;
	buffLen = cnt;

	// Set slave address, read transfer, number of bytes to read, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | (buffLen << 16) | (slaveAddr << 1);

	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= I2C_TransferTimeout){
			status = Status::Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_RTimeout, (uint16_t)slaveAddr);
		}
	}

	return status;
}

// -----------------------------------------------------------------------------

CPU_I2C::Status CPU_I2C::WriteByte(uint8_t slaveAddr, uint8_t data)
{
	buffer[0] = data;
	return WriteBuffer(slaveAddr, 1);
}

CPU_I2C::Status CPU_I2C::Write2Bytes(uint8_t slaveAddr, uint8_t data0, uint8_t data1)
{
	buffer[0] = data0;
	buffer[1] = data1;
	return WriteBuffer(slaveAddr, 2);
}

CPU_I2C::Status CPU_I2C::WriteBuffer(uint8_t slaveAddr, uint8_t cnt)
{
	if((status == Status::Disabled) || (status == Status::IntfErr)) Enable();

	status = Status::OK;
	transferDone = false;
	buffIdx = 0;
	buffLen = cnt;

	// Set slave address, write transfer, number bytes to send, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | (buffLen << 16) | (slaveAddr << 1);

	I2C1->TXDR = buffer[buffIdx++];
	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= I2C_TransferTimeout){
			status = Status::Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_WTimeout, (uint16_t)slaveAddr);
		}
	}

	return status;
}

// -----------------------------------------------------------------------------

CPU_I2C::Status CPU_I2C::WriteByteAndRead(uint8_t slaveAddr, uint8_t data, uint8_t rdCnt)
{
	buffer[0] = data;
	return WriteAndRead(slaveAddr, 1, rdCnt);
}

CPU_I2C::Status CPU_I2C::WriteAndRead(uint8_t slaveAddr, uint8_t wrCnt, uint8_t rdCnt)
{
	if((status == Status::Disabled) || (status == Status::IntfErr)) Enable();

	status = Status::OK;
	transferDone = false;
	buffIdx = 0;
	buffLen = wrCnt;

	// Set slave address, write transfer, number bytes to send, NO autoend
	I2C1->CR2 = (buffLen << 16) | (slaveAddr << 1);

	I2C1->TXDR = buffer[buffIdx++];
	I2C1->CR2 |= I2C_CR2_START;

	uint32_t timeStart = sysTickCnt;
	while((I2C1->ISR & I2C_ISR_TC) != I2C_ISR_TC) {
		if((sysTickCnt - timeStart) >= I2C_TransferTimeout){
			status = Status::Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_CWTimeout, (uint16_t)slaveAddr);
		}
	}

	if(status != Status::OK) return status;

	buffIdx = 0;
	buffLen = rdCnt;

	// Set slave address, read transfer, number of bytes to read, autoend
	I2C1->CR2 = I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | (buffLen << 16) | (slaveAddr << 1);

	I2C1->CR2 |= I2C_CR2_START;

	timeStart = sysTickCnt;
	while(!transferDone) {
		if((sysTickCnt - timeStart) >= I2C_TransferTimeout){
			status = Status::Timeout;
			transferDone = true;
			hwLogger.AddEntry(FileID, LOG_CODE_CRTimeout, (uint16_t)slaveAddr);
		}
	}

	return status;
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
		status = Status::NACK;
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
				// was slave transmitter
				// disable TXIE interrupt and flush I2C_TXDR register
				I2C1->CR1 &= ~I2C_CR1_TXIE;
				I2C1->ISR |= I2C_ISR_TXE;
			}
		}
	}
	else {
		status = Status::IntfErr;
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
