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

#ifndef dev_HIHSENSOR_H_
#define dev_HIHSENSOR_H_

#include "cpu_I2C.h"

namespace PAx5 {

class DEV_HIHSensor
{
public:
	DEV_HIHSensor();
	virtual ~DEV_HIHSensor();

	enum class Status : uint8_t {
		DATA_OK,
		DATA_Stale,
		DATA_NACK,
		DATA_IntfErr
	};

	uint16_t rawH, rawT;

	/**
	 * \brief Begin measurement cycle
	 *
	 * \details Call this function to start a measurement cycle.
	 * The measurement cycle duration is typically 36.65 ms for temperature and humidity readings.
	 *
	 * \return Status::DATA_OK is sensor acknowledged the packet
	 *         Status::DATA_IntfErr is sensor not present
	 */
	DEV_HIHSensor::Status ReadInit(void);

	/**
	 * \brief Read measured data
	 *
	 * \return Status::DATA_OK if new data was read
	 * \return Status::DATA_Stale if old data was read
	 * \return Status::DATA_NACK or HIH_DATA_IntfErr if error (wrong value or sensor not present)
	 */
	DEV_HIHSensor::Status ReadData(void);

	/**
	 * \brief Converts rawH to 10*humidity[%RH]
	 *
	 * rawH interval is [0 ... (2^14 - 2)] for [0 ... 100] %RH
	 * output interval is [0 ... 1000]
	 */
	uint16_t GetHumidity(void);

	/** \brief Converts rawT to 10*temperature[degK]
	 *
	 * rawT interval is [0 ... (2^14 - 2)] for [-40 ... +125] degC
	 * output interval is [2331 ... 3981]. To convert in 10*degC substract 2731
	 */
	uint16_t GetTemperature(void);
};

} /* namespace */
#endif
