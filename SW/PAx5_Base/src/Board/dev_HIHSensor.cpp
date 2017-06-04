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

#include <dev_HIHSensor.h>

namespace PAx5 {
// -----------------------------------------------------------------------------

DEV_HIHSensor::DEV_HIHSensor() {
	rawH = 0;
	rawT = 0;
}

DEV_HIHSensor::~DEV_HIHSensor() {
}

// -----------------------------------------------------------------------------

DEV_HIHSensor::Status DEV_HIHSensor::ReadInit(void)
{
	if(sI2C.Test(0x27) == CPU_I2C::Status::OK)
		return Status::DATA_OK;

	return Status::DATA_IntfErr;
}

DEV_HIHSensor::Status DEV_HIHSensor::ReadData(void)
{
	CPU_I2C::Status i2cState = sI2C.Read(0x27, 4);
	if(i2cState == CPU_I2C::Status::OK){
		uint8_t stale = sI2C.buffer[0] & 0xC0;

		rawH  = sI2C.buffer[0] & 0x3F;
		rawH  = rawH << 8;
		rawH += sI2C.buffer[1];

		rawT  = sI2C.buffer[2];
		rawT  = rawT << 8;
		rawT += sI2C.buffer[3];
		rawT  = rawT >> 2;

		if(stale == 0x00) return Status::DATA_OK;
		if(stale == 0x40) return Status::DATA_Stale;
		return Status::DATA_IntfErr;
	}

	if(i2cState == CPU_I2C::Status::NACK)
		return Status::DATA_NACK;
	return Status::DATA_IntfErr;
}

// -----------------------------------------------------------------------------

uint16_t DEV_HIHSensor::GetHumidity(void)
{
	uint32_t val;

	val = rawH;
	val *= 5000;
	val /= 8191;
	val += 5;
	val /= 10;
	return (uint16_t)val;
}

uint16_t DEV_HIHSensor::GetTemperature(void)
{
	uint32_t val;

	val = rawT;
	val *= 8250;
	val /= 8191;
	val += 23310;
	val += 5;
	val /= 10;
	return (uint16_t)val;
}

// -----------------------------------------------------------------------------
} /* namespace */
