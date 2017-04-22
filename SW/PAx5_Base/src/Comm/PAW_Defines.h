/**
 * created 2016.07.26 by Calin Radoni
 */

#ifndef PAW_DEFINES_H_
#define PAW_DEFINES_H_

#include <stddef.h>
#include <stdint.h>

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

namespace AddressClass {
	const uint8_t Address_NONE      = 0;
	const uint8_t Address_Gateway   = 1;
	const uint8_t Address_Broadcast = 255;
}

// -----------------------------------------------------------------------------

namespace Commands {
	const uint8_t Command_NOP            = 0;

	const uint8_t Command_Firmware_Begin = 10;
	const uint8_t Command_Firmware_Data  = 11;
	const uint8_t Command_Firmware_End   = 12;

	const uint8_t Command_Get_Status     = 20;

	const uint8_t Command_Get_Channels   = 30;
	const uint8_t Command_Get_Channel    = 31;
	const uint8_t Command_Set_Channel    = 32;
}

// -----------------------------------------------------------------------------

namespace PAx5CommProtocol {
	enum ChannelType {
		ctFirmware      = 0x00,
		ctPower         = 0x01,

		ctContact       = 0x10,
		ctSwitch        = 0x11,
		ctDimmer        = 0x12,
		ctBlinds        = 0x13,
		ctColor         = 0x14,
		ctDateTime      = 0x15,

		ctTemperature   = 0x20,
		ctHumidity      = 0x21,
		ctLight         = 0x22,
		ctLightUV       = 0x23,
		ctLightIR       = 0x24,
		ctWindSpeed     = 0x25,
		ctWindDirection = 0x26,
		ctRain          = 0x27,
		ctWater         = 0x28,
		ctRadiation     = 0x29,
		ctGasGeneric    = 0x2A,
		ctGasCO         = 0x2B,
		ctGasCO2        = 0x2C,
		ctGasCH4        = 0x2D,

		ctAlarm         = 0x30,
		ctMotion        = 0x31,
		ctSmoke         = 0x32,
		ctFire          = 0x33,

		ctPlayer        = 0x40,

		ctLogSize       = 0xFC,
		ctLogEntry      = 0xFD,
		ctExtended      = 0xFE,
		ctEND           = 0xFF
	};
}
// -----------------------------------------------------------------------------

} /* namespace */

#endif
