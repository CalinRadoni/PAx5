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

#ifndef dev_TSL2561_H_
#define dev_TSL2561_H_

#include "cpu_I2C.h"

namespace PAx5 {

/**
 * From TAOS059N - MARCH 2009, page 19:
 * After applying VDD , the device will initially be in the power-down
 * state. To operate the device, issue a command to access the CONTROL
 * register followed by the data value 03h to power up the device. At this
 * point, both ADC channels will begin a conversion at the default
 * integration time of 400 ms. After 400 ms, the conversion results will
 * be available in the DATA0 and DATA1 registers.
 */

class DEV_TSL2561
{
public:
	DEV_TSL2561();
	virtual ~DEV_TSL2561();

	enum class Status : uint8_t {
		OK,
		NACK,
		IntfErr
	};

	enum class Address : uint8_t {
		AddressLow_x29,   // Address pin connected to GND
		AddressFloat_x39, // Address pin unconnected
		AddressHigh_x49   // Address pin connected to VDD
	};
	enum class IntegrationTime : uint8_t {
		IntegrationTime_14ms,
		IntegrationTime_101ms,
		IntegrationTime_402ms
	};

	/**
	 * \brief Check if the chip is present at supplied address
	 *
	 * \return Status::OK if the sensor is present and ID register's value is OK
	 *         Status::NACK if there is no I2C device at the specified address
	 *         Status::IntfErr in all other cases
	 */
	DEV_TSL2561::Status Check(Address);

	/**
	 * \brief Set the chip
	 *
	 * \details This functions wakes the chip, set it's parameters then put the chip to sleep.
	 * After applying VDD:
	 * - Integration time is 402 ms
	 * - gain is 1x
	 */
	DEV_TSL2561::Status Set(Address, IntegrationTime, bool gain16x);

	DEV_TSL2561::Status WakeUp(void);
	DEV_TSL2561::Status Sleep(void);
	DEV_TSL2561::Status SetGain(bool gain16x);

	/**
	 * \brief Get the value of sensor's ID register
	 *
	 * \details The ID register is:
	 * - ID[7 - 4] = Part number
	 * - ID[3 - 0] = Revision number
	 * Part number is 0 for TSL2560 and 1 for TSL2561
	 *
	 * \return the value read from the ID register
	 *         0xFF on error
	 */
	uint8_t GetID(void);

private:
	uint8_t sensorAddress;
	uint8_t intTime;
	uint8_t intGain;
};

} /* namespace */

#endif /* BOARD_DEV_TSL2561_H_ */
