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

#include <dev_TSL2561.h>

namespace PAx5 {

const uint8_t RegCommand_Cmd      = 0x80;
const uint8_t RegCommand_ClearInt = 0x40;

const uint8_t RegControl   = 0x00; // Control of basic functions
const uint8_t RegTiming    = 0x01; // Integration time/gain control
const uint8_t RegThrLowL   = 0x02; // Low byte of low interrupt threshold
const uint8_t RegThrLowH   = 0x03; // High byte of low interrupt threshold
const uint8_t RegThrHighL  = 0x04; // Low byte of high interrupt threshold
const uint8_t RegThrHighH  = 0x05; // High byte of high interrupt threshold
const uint8_t RegInterrupt = 0x06; // Interrupt control
const uint8_t RegCRC       = 0x08; // Factory test - not a user register
const uint8_t RegID        = 0x0A; // Part number / Rev ID
const uint8_t RegData0L    = 0x0C; // Low byte of ADC channel 0
const uint8_t RegData0H    = 0x0D; // High byte of ADC channel 0
const uint8_t RegData1L    = 0x0E; // Low byte of ADC channel 1
const uint8_t RegData1H    = 0x0F; // High byte of ADC channel 1

const uint8_t RegControl_PowerUp   = 0x03;
const uint8_t RegControl_PowerDown = 0x00;

const uint8_t RegTiming_Gain1x  = 0x00;
const uint8_t RegTiming_Gain16x = 0x10;
const uint8_t RegTiming_IT_14   = 0x00; // 13.7 ms, scale = 0.034
const uint8_t RegTiming_IT_101  = 0x01; // 101 ms,  scale = 0.252
const uint8_t RegTiming_IT_402  = 0x02; // 402 ms,  scale = 1

const uint8_t RegInt_IntDisabled = 0x00;
const uint8_t RegInt_IntLevel    = 0x10;
const uint8_t RegInt_IntSMBAlert = 0x20;
const uint8_t RegInt_IntTest     = 0x30;
const uint8_t RegInt_PersistEveryADC = 0x00;
//const uint8_t RegInt_PersistOutsideThr1 = 0x01;
// ...
//const uint8_t RegInt_PersistOutsideThrF = 0x0F;

// -----------------------------------------------------------------------------

DEV_TSL2561::DEV_TSL2561() {
	sensorAddress = 0x39;
	intTime = RegTiming_IT_402;
	intGain = RegTiming_Gain1x;
}

DEV_TSL2561::~DEV_TSL2561() {
}

// -----------------------------------------------------------------------------

DEV_TSL2561::Status DEV_TSL2561::Check(Address addr)
{
	uint8_t sa = 0x39; // initialized here to avoid a warning in 'switch'

	switch(addr){
		case Address::AddressLow_x29:   sa = 0x29; break;
		case Address::AddressFloat_x39: sa = 0x39; break;
		case Address::AddressHigh_x49:  sa = 0x49; break;
	}

	sI2C.buffLen = 0;
	if(sI2C.Write(sa) != CPU_I2C::Status::OK) return Status::NACK;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerUp;
	if(sI2C.Write(sa) != CPU_I2C::Status::OK) return Status::IntfErr;

	sI2C.buffLen = 1;
	sI2C.buffer[0] = RegID;
	if(sI2C.WriteAndRead(sa, 1, 1) != CPU_I2C::Status::OK) return Status::IntfErr;
	uint8_t data = sI2C.buffer[0];
	if((data & 0xF0) != 0x10)
		return Status::IntfErr;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerDown;
	if(sI2C.Write(sa) != CPU_I2C::Status::OK) return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::Set(Address addr, IntegrationTime time, bool gain16x)
{
	switch(addr){
		case Address::AddressLow_x29:   sensorAddress = 0x29; break;
		case Address::AddressFloat_x39: sensorAddress = 0x39; break;
		case Address::AddressHigh_x49:  sensorAddress = 0x49; break;
	}

	switch(time){
		case IntegrationTime::IntegrationTime_14ms:  intTime = RegTiming_IT_14;  break;
		case IntegrationTime::IntegrationTime_101ms: intTime = RegTiming_IT_101; break;
		case IntegrationTime::IntegrationTime_402ms: intTime = RegTiming_IT_402; break;
	}

	if(gain16x) intGain = RegTiming_Gain16x;
	else        intGain = RegTiming_Gain1x;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerUp;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK) return Status::IntfErr;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegTiming;
	sI2C.buffer[1] = intTime | intGain;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK) return Status::IntfErr;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerDown;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK) return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::WakeUp(void)
{
	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerUp;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::Sleep(void)
{
	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegControl;
	sI2C.buffer[1] = RegControl_PowerUp;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::SetGain(bool gain16x)
{
	if(gain16x) intGain = RegTiming_Gain16x;
	else        intGain = RegTiming_Gain1x;

	sI2C.buffLen = 2;
	sI2C.buffer[0] = RegCommand_Cmd | RegTiming;
	sI2C.buffer[1] = intTime | intGain;
	if(sI2C.Write(sensorAddress) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

uint8_t DEV_TSL2561::GetID(void)
{
	sI2C.buffLen = 1;
	sI2C.buffer[0] = RegID;
	if(sI2C.WriteAndRead(sensorAddress, 1, 1) != CPU_I2C::Status::OK)
		return 0xFF;

	return sI2C.buffer[0];
}

// -----------------------------------------------------------------------------
} /* namespace */