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

#ifndef cpu_I2C_H_
#define cpu_I2C_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t I2CBufferLen = 16;

/**
 * CPU_I2C is the driver class for I2C communications.
 * It can be initialized as master with the {\see CPU_I2C#Enable Enable()} function
 * or as slave with the {\see CPU_I2C#EnableAsSlave EnableAsSlave} function.
 *
 * This class uses I2C1 peripheral.
 */
class CPU_I2C {
public:
	CPU_I2C();

	enum class Status : uint8_t {
		OK,
		NACK,
		IntfErr,
		Timeout,
		Disabled
	};

	/** \brief Enable as master */
	void Enable(void);

	/**
	 * \brief Enable as slave
	 * \param address to be used as I2C slave device
	 */
	void EnableAsSlave(uint8_t); // address

	/** Disable I2C peripheral */
	void Disable(void);

	/**
	 * \brief Communication buffer
	 * \details This buffer is used for transferring data between system and I2C bus
	 */
	volatile uint8_t buffer[I2CBufferLen];

	/**
	 * \brief Test if is an I2C slave at supplied address
	 *
	 * \return Status::OK if a slave device is present
	 *         Status::NACK if a slave is not present
	 *         Other values on error
	 * At the end #buffLen contains the number of bytes read.
	 */
	CPU_I2C::Status Test(uint8_t slaveAddr);

	/**
	 * \brief Read from an I2C slave
	 *
	 * \details Reads {\link #buffLen} bytes from the {\link #buffer buffer[]}
	 * At the end #buffLen contains the number of bytes read.
	 */
	CPU_I2C::Status Read(uint8_t slaveAddr, uint8_t cnt);

	/** Write a byte to an I2C slave.
	 * This is a shortcut function that calls #WriteBuffer function
	 */
	CPU_I2C::Status WriteByte(uint8_t slaveAddr, uint8_t data);

	/** Write two bytes to an I2C slave.
	 * This is a shortcut function that calls #WriteBuffer function
	 */
	CPU_I2C::Status Write2Bytes(uint8_t slaveAddr, uint8_t data0, uint8_t data1);

	/**
	 * \brief Write from #buffer to an I2C slave
	 *
	 * \details Write specified number of bytes from #buffer
	 *
	 * \return
	 */
	CPU_I2C::Status WriteBuffer(uint8_t slaveAddr, uint8_t cnt);

	/**
	 * Write a byte and read some using repeated start
	 * This is a shortcut function that calls #WriteAndRead function
	 */
	CPU_I2C::Status WriteByteAndRead(uint8_t slaveAddr, uint8_t data, uint8_t rdCnt);

	/**
	 * \brief Write and read using repeated start
	 * \details Combine write and read using repeated start condition.
	 * \param slaveAddr address of I2C slave
	 * \param wrCnt     number of bytes to write
	 * \param rdCnt     number of bytes to read
	 */
	CPU_I2C::Status WriteAndRead(uint8_t slaveAddr, uint8_t wrCnt, uint8_t rdCnt);

	/**
	 * \brief User function to handle received data in SLAVE mode
	 * \details For SLAVE mode only.
	 */
	void onReceive(void(*)(uint8_t));

	/**
	 * \brief User function called before sending data in SLAVE mode
	 * \details For SLAVE mode only.
	 */
	void onRequest(void(*)(void));

	/**
	 * \brief The I2C1 interrupt handler
	 */
	void HandleI2CInt(void);

protected:
	volatile uint8_t buffLen;
	volatile uint8_t buffIdx;
	volatile bool transferDone;

	volatile uint8_t myAddress;

	volatile Status status;

	// master of slave mode
	volatile bool masterMode;

	// in slave mode, it is receiver or transmitter
	volatile bool slaveReceiver;

    void (*callbackOnReceive)(uint8_t);
    void (*callbackOnRequest)(void);
};

extern CPU_I2C sI2C;

}
#endif /* cpu_I2C_H_ */
