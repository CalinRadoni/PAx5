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
const uint8_t RegCommand_Word     = 0x20;

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

const uint32_t WaitTime_14  =  16;
const uint32_t WaitTime_101 = 120;
const uint32_t WaitTime_402 = 450;

const uint8_t RegInt_IntDisabled = 0x00;
const uint8_t RegInt_IntLevel    = 0x10;
const uint8_t RegInt_IntSMBAlert = 0x20;
const uint8_t RegInt_IntTest     = 0x30;
const uint8_t RegInt_PersistEveryADC = 0x00;
//const uint8_t RegInt_PersistOutsideThr1 = 0x01;
// ...
//const uint8_t RegInt_PersistOutsideThrF = 0x0F;

const uint32_t Return_Error = 0xFFFE;
const uint32_t Return_Clipping = 0xFFFF;

// -----------------------------------------------------------------------------

DEV_TSL2561::DEV_TSL2561() {
	sensorAddress = 0x39;
	intTime       = RegTiming_IT_402;
	startTime     = sysTickCnt;
	waitTime      = WaitTime_402;
	intGain       = RegTiming_Gain1x;
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

	if(sI2C.Test(sa) != CPU_I2C::Status::OK)
		return Status::NACK;

	if(sI2C.Write2Bytes(sa, RegCommand_Cmd | RegControl, RegControl_PowerUp) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	if(sI2C.WriteByteAndRead(sa, RegID, 1) != CPU_I2C::Status::OK)
		return Status::IntfErr;
	uint8_t data = sI2C.buffer[0];
	if((data & 0xF0) != 0x10)
		return Status::IntfErr;

	if(sI2C.Write2Bytes(sa, RegCommand_Cmd | RegControl, RegControl_PowerDown) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::Set(Address addr, IntegrationTime time, bool gain16x)
{
	switch(addr){
		case Address::AddressLow_x29:   sensorAddress = 0x29; break;
		case Address::AddressFloat_x39: sensorAddress = 0x39; break;
		case Address::AddressHigh_x49:  sensorAddress = 0x49; break;
	}

	if(sI2C.Write2Bytes(sensorAddress, RegCommand_Cmd | RegControl, RegControl_PowerUp) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	if(SetTimeAndGain(time, gain16x) != Status::OK)
		return Status::IntfErr;

	if(sI2C.Write2Bytes(sensorAddress, RegCommand_Cmd | RegControl, RegControl_PowerDown) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::SetTimeAndGain(IntegrationTime time, bool gain16x)
{
	switch(time){
		case IntegrationTime::IntegrationTime_14ms:
			intTime = RegTiming_IT_14;
			waitTime = WaitTime_14;
			break;
		case IntegrationTime::IntegrationTime_101ms:
			intTime = RegTiming_IT_101;
			waitTime = WaitTime_101;
			break;
		case IntegrationTime::IntegrationTime_402ms:
			intTime = RegTiming_IT_402;
			waitTime = WaitTime_402;
			break;
	}

	if(gain16x) intGain = RegTiming_Gain16x;
	else        intGain = RegTiming_Gain1x;

	if(sI2C.Write2Bytes(sensorAddress, RegCommand_Cmd | RegTiming, intTime | intGain) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

// -----------------------------------------------------------------------------

DEV_TSL2561::Status DEV_TSL2561::WakeUp(void)
{
	if(sI2C.Write2Bytes(sensorAddress, RegCommand_Cmd | RegControl, RegControl_PowerUp) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	startTime = sysTickCnt;
	return Status::OK;
}

DEV_TSL2561::Status DEV_TSL2561::Sleep(void)
{
	if(sI2C.Write2Bytes(sensorAddress, RegCommand_Cmd | RegControl, RegControl_PowerDown) != CPU_I2C::Status::OK)
		return Status::IntfErr;

	return Status::OK;
}

// -----------------------------------------------------------------------------

uint8_t DEV_TSL2561::GetID(void)
{
	if(sI2C.WriteByteAndRead(sensorAddress, RegID, 1) != CPU_I2C::Status::OK)
		return 0xFF;

	return sI2C.buffer[0];
}

// -----------------------------------------------------------------------------

bool DEV_TSL2561::DataIsReady(void)
{
	return ((sysTickCnt - startTime) >= waitTime);
}

bool DEV_TSL2561::ReadData(void)
{
	uint8_t dH, dL;

	if(sI2C.WriteByteAndRead(sensorAddress, RegCommand_Cmd | RegCommand_Word | RegData0L, 2) != CPU_I2C::Status::OK)
		return false;
	dL = sI2C.buffer[0];
	dH = sI2C.buffer[1];
	rawLight = dH;
	rawLight = rawLight << 8;
	rawLight |= dL;

	if(sI2C.WriteByteAndRead(sensorAddress, RegCommand_Cmd | RegCommand_Word | RegData1L, 2) != CPU_I2C::Status::OK)
		return false;
	dL = sI2C.buffer[0];
	dH = sI2C.buffer[1];
	rawIR = dH;
	rawIR = rawIR << 8;
	rawIR |= dL;

	return true;
}

bool DEV_TSL2561::Restart(void)
{
	if(Sleep() != Status::OK)
		return false;

	// wait 1 - 2 ms
	startTime = sysTickCnt;
	while((sysTickCnt - startTime) < 2) { /* wait */ }

	if(WakeUp() != Status::OK)
		return false;

	return true;
}

// -----------------------------------------------------------------------------

const uint8_t TSL2561_LUX_SCALE   = 14; // scale by 2^14
const uint8_t TSL2561_RATIO_SCALE =  9; // scale ratio by 2^9
const uint8_t TSL2561_CH_SCALE    = 10; // scale channel values by 2^10

const uint32_t TSL2561_CHSCALE_TINT0 = 0x7517; // 322/11 * 2^CH_SCALE
const uint32_t TSL2561_CHSCALE_TINT1 = 0x0fe7; // 322/81 * 2^CH_SCALE

const uint32_t TSL2561_K1T = 0x0040; // 0.125 * 2^RATIO_SCALE
const uint32_t TSL2561_B1T = 0x01f2; // 0.0304 * 2^LUX_SCALE
const uint32_t TSL2561_M1T = 0x01be; // 0.0272 * 2^LUX_SCALE
const uint32_t TSL2561_K2T = 0x0080; // 0.250 * 2^RATIO_SCALE
const uint32_t TSL2561_B2T = 0x0214; // 0.0325 * 2^LUX_SCALE
const uint32_t TSL2561_M2T = 0x02d1; // 0.0440 * 2^LUX_SCALE
const uint32_t TSL2561_K3T = 0x00c0; // 0.375 * 2^RATIO_SCALE
const uint32_t TSL2561_B3T = 0x023f; // 0.0351 * 2^LUX_SCALE
const uint32_t TSL2561_M3T = 0x037b; // 0.0544 * 2^LUX_SCALE
const uint32_t TSL2561_K4T = 0x0100; // 0.50 * 2^RATIO_SCALE
const uint32_t TSL2561_B4T = 0x0270; // 0.0381 * 2^LUX_SCALE
const uint32_t TSL2561_M4T = 0x03fe; // 0.0624 * 2^LUX_SCALE
const uint32_t TSL2561_K5T = 0x0138; // 0.61 * 2^RATIO_SCALE
const uint32_t TSL2561_B5T = 0x016f; // 0.0224 * 2^LUX_SCALE
const uint32_t TSL2561_M5T = 0x01fc; // 0.0310 * 2^LUX_SCALE
const uint32_t TSL2561_K6T = 0x019a; // 0.80 * 2^RATIO_SCALE
const uint32_t TSL2561_B6T = 0x00d2; // 0.0128 * 2^LUX_SCALE
const uint32_t TSL2561_M6T = 0x00fb; // 0.0153 * 2^LUX_SCALE
const uint32_t TSL2561_K7T = 0x029a; // 1.3 * 2^RATIO_SCALE
const uint32_t TSL2561_B7T = 0x0018; // 0.00146 * 2^LUX_SCALE
const uint32_t TSL2561_M7T = 0x0012; // 0.00112 * 2^LUX_SCALE
const uint32_t TSL2561_B8T = 0x0000; // 0.000 * 2^LUX_SCALE
const uint32_t TSL2561_M8T = 0x0000; // 0.000 * 2^LUX_SCALE

const uint16_t TSL2561_CLIP_14 = 5000;
const uint16_t TSL2561_CLIP_101 = 37000;
const uint16_t TSL2561_CLIP_178 = 65000;

bool DEV_TSL2561::Clipping(void)
{
	// according to datasheet:
	// - Tint = 13.7 ms, max ADC count is 5047
	// - Tint = 101 ms, max ADC count is 37177
	// - Tint > 178 ms, max ADC count is 65535, limited by the 16 bit registers

	bool clipping = false;

	switch (intTime) {
	case RegTiming_IT_14:
		if((rawLight > TSL2561_CLIP_14) || (rawIR > TSL2561_CLIP_14)) clipping = true;
		break;
	case RegTiming_IT_101:
		if((rawLight > TSL2561_CLIP_101) || (rawIR > TSL2561_CLIP_101)) clipping = true;
		break;
	default: // assume no scaling
		if((rawLight > TSL2561_CLIP_178) || (rawIR > TSL2561_CLIP_178)) clipping = true;
		break;
	}

	return clipping;
}

uint32_t DEV_TSL2561::ConvertInLux(void)
{
	if(Clipping())
		return 0xFFFF;

	// Scale the raw values depending on the gain and integration time. 16x and 402ms are nominal values.
	uint32_t chScale;

	switch (intTime) {
	case RegTiming_IT_14:
		chScale = TSL2561_CHSCALE_TINT0;
		break;
	case RegTiming_IT_101:
		chScale = TSL2561_CHSCALE_TINT1;
		break;
	default:
		chScale = (1U << TSL2561_CH_SCALE); // no scaling
		break;
	}

	if(intGain == RegTiming_Gain1x)
		chScale = chScale << 4; // scale 1x to 16x

	// maximum chScale values are: 0x75170, 0xFE70 and 0x4000
	// for RegTiming_IT_14 and scale 1x maximum allowed value for rawXXX is 8955
	// according to datasheet:
	// - Tint = 13.7 ms, max ADC count is 5047
	// - Tint = 101 ms, max ADC count is 37177
	// - Tint > 178 ms, max ADC count is 65535, limited by the 16 bit registers
	// so no overflow in this function

	uint32_t channel0 = (rawLight * chScale) >> TSL2561_CH_SCALE;
	uint32_t channel1 = (rawIR * chScale)    >> TSL2561_CH_SCALE;

	// find the ratio of the channel values (Channel1/Channel0)
	uint32_t ratio = 0;
	if(channel0 != 0)
		ratio = (channel1 << (TSL2561_RATIO_SCALE + 1)) / channel0;
	// round the ratio value
	ratio = (ratio + 1) >> 1;

	uint32_t b, m;
		 if (ratio <= TSL2561_K1T) {b = TSL2561_B1T; m = TSL2561_M1T;}
	else if (ratio <= TSL2561_K2T) {b = TSL2561_B2T; m = TSL2561_M2T;}
	else if (ratio <= TSL2561_K3T) {b = TSL2561_B3T; m = TSL2561_M3T;}
	else if (ratio <= TSL2561_K4T) {b = TSL2561_B4T; m = TSL2561_M4T;}
	else if (ratio <= TSL2561_K5T) {b = TSL2561_B5T; m = TSL2561_M5T;}
	else if (ratio <= TSL2561_K6T) {b = TSL2561_B6T; m = TSL2561_M6T;}
	else if (ratio <= TSL2561_K7T) {b = TSL2561_B7T; m = TSL2561_M7T;}
	else                           {b = TSL2561_B8T; m = TSL2561_M8T;}

	uint32_t t0 = channel0 * b;
	uint32_t t1 = channel1 * m;
	uint32_t result;

	if(t0 <= t1) result = 0;
	else{
		result = t0 - t1;

		result += (1U << (TSL2561_LUX_SCALE - 1)); // round lsb (2 ^ (LUX_SCALE - 1))
		result = result >> TSL2561_LUX_SCALE;      // strip fractional part
	}

	return result;
}

// -----------------------------------------------------------------------------

uint32_t DEV_TSL2561::GetLux(Address addr)
{
	uint32_t res;

	// try 101ms and 16x

	if(Set(addr, IntegrationTime::IntegrationTime_101ms, true) != Status::OK)
		return Return_Error;
	if(WakeUp() != Status::OK)
		return Return_Error;
	while(!DataIsReady()) { /* wait */ }
	if(!Clipping()){
		res = ConvertInLux();
		Sleep();
		return res;
	}

	// try 101ms and 1x
	if(SetTimeAndGain(IntegrationTime::IntegrationTime_101ms, false) != Status::OK){
		Sleep();
		return Return_Error;
	}
	if(!Restart()){
		Sleep();
		return Return_Error;
	}
	while(!DataIsReady()) { /* wait */ }
	if(!Clipping()){
		res = ConvertInLux();
		Sleep();
		return res;
	}

	// try 13.7ms and 16x
	if(SetTimeAndGain(IntegrationTime::IntegrationTime_14ms, true) != Status::OK){
		Sleep();
		return Return_Error;
	}
	if(!Restart()){
		Sleep();
		return Return_Error;
	}
	while(!DataIsReady()) { /* wait */ }
	if(!Clipping()){
		res = ConvertInLux();
		Sleep();
		return res;
	}

	// try 13.7ms and 1x
	if(SetTimeAndGain(IntegrationTime::IntegrationTime_14ms, false) != Status::OK){
		Sleep();
		return Return_Error;
	}
	if(!Restart()){
		Sleep();
		return Return_Error;
	}
	while(!DataIsReady()) { /* wait */ }
	if(!Clipping()){
		res = ConvertInLux();
		Sleep();
		return res;
	}

	// sensor limit reached
	Sleep();
	return Return_Clipping;
}

// -----------------------------------------------------------------------------
} /* namespace */
