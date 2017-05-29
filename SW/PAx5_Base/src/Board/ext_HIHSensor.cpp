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

#include "ext_HIHSensor.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

EXT_HIHSensor::EXT_HIHSensor() {
	rawH = 0;
	rawT = 0;
}

EXT_HIHSensor::~EXT_HIHSensor() {
}

// -----------------------------------------------------------------------------

EXT_HIHSensor::Status EXT_HIHSensor::ReadInit(void)
{
	sI2C.buffLen = 0;
	sI2C.Write(0x27);
	if(sI2C.status == I2CStatus_OK)
		return Status::DATA_OK;
	return Status::DATA_IntfErr;
}

EXT_HIHSensor::Status EXT_HIHSensor::ReadData(void)
{
	uint8_t stale;
	sI2C.buffLen = 4;
	sI2C.Read(0x27);
	if(sI2C.status == I2CStatus_OK){
		stale = sI2C.buffer[0] & 0xC0;

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

	if(sI2C.status == I2CStatus_NACK)
		return Status::DATA_NACK;
	return Status::DATA_IntfErr;
}

// -----------------------------------------------------------------------------

uint16_t EXT_HIHSensor::GetHumidity(void)
{
	uint32_t val;

	val = rawH;
	val *= 5000;
	val /= 8191;
	val += 5;
	val /= 10;
	return (uint16_t)val;
}

uint16_t EXT_HIHSensor::GetTemperature(void)
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
}
