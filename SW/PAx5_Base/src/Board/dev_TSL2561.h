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

	DEV_TSL2561::Status SetTimeAndGain(IntegrationTime, bool gain16x);

	/**
	 * Wakes up the sensor.
	 * Also starts data acquisition.
	 */
	DEV_TSL2561::Status WakeUp(void);

	DEV_TSL2561::Status Sleep(void);

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

	/**
	 * If enough time has elapsed since sensor power up this function returns true.
	 */
	bool DataIsReady(void);

	/**
	 * Read the channel 0 (Visible + IR) and channel 1 (IR only).
	 * The data goes to #rawLight and #rawIR
	 */
	bool ReadData(void);

	/**
	 * Put the sensor to sleep, wait at least 1ms and wakes the sensor up
	 */
	bool Restart(void);

	/**
	 * Returns true if acquired values are closer to the maximum value
	 * corresponding to current integration time
	 */
	bool Clipping(void);

	/**
	 * Converts the raw values set by #ReadData function to Lux
	 * This function is adapted from TSL2561 datasheet (TAOS059N - March 2009)
	 *
	 * \warning The value is good only for T, F and CL packages (no CS package).
	 *
	 * \return the light intensity in Lux
	 *         0xFFFF if clipping
	 */
	uint32_t ConvertInLux(void);

	/**
	 * Simple function to read and convert data.
	 * This function:
	 * - set the sensor
	 * - makes a read at 101 ms integration time and 16x gain
	 * - if the value is clipped makes another read at 101 ms integration time and 1x gain
	 * - convert raw values to lux
	 * - put the sensor to sleep
	 *
	 * \warning This is a blocking function ! It takes between 120 ms and 250 ms to return.
	 *
	 * \return the light intensity in Lux if acquired values are usable
	 *         0xFFFF if clipping
	 *         0xFFFE if error
	 */
	uint32_t GetLux(Address);

private:
	uint8_t sensorAddress;
	uint8_t intTime;
	uint8_t intGain;

	uint32_t startTime;
	uint32_t waitTime;

	uint32_t rawLight;
	uint32_t rawIR;
};

} /* namespace */

#endif /* BOARD_DEV_TSL2561_H_ */
