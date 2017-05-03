/**
 * created 2016.04.25 by Calin Radoni
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
