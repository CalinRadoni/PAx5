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

	void Enable(void);
	void EnableAsSlave(uint8_t); // address
	void Disable(void);

	volatile uint8_t buffer[I2CBufferLen];
	volatile uint8_t buffLen;
	volatile bool transferDone;

	CPU_I2C::Status Read(uint8_t); // slave address
	CPU_I2C::Status Write(uint8_t); // slave address

	// write and read using repeated start
	CPU_I2C::Status WriteAndRead(uint8_t, uint8_t, uint8_t); // slave address, no. of bytes to write, no. of bytes to read

	// for slave mode only, called after data has been received
	void onReceive(void(*)(uint8_t));

	// for slave mode only, called before sending data
	void onRequest(void(*)(void));

	void HandleI2CInt(void);

protected:
	volatile uint8_t buffIdx;
	uint8_t myAddress;

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
