/**
 * created 2016.11.27 by Calin Radoni
 */

#include <version.h>
#include "GW_Config.h"
#include "cpu_MemoryEEPROM.h"
#include "CommDefines.h"

namespace PAx5Gateway {

// -----------------------------------------------------------------------------

#define EEPROM_ADDR_INC 4

#define EEPROM_LOC_ConfigInfo  (0 << 2)
#define EEPROM_LOC_NetKeyStart (1 << 2)
#define EEPROM_LOC_Frequency   (9 << 2)

// -----------------------------------------------------------------------------

GW_Config::GW_Config()
{
	configInfo.verHi = 0;
	configInfo.verLo = 0;
	configInfo.address = PAx5CommProtocol::AddressClass::Address_NONE;
}

// -----------------------------------------------------------------------------

void GW_Config::ReadConfigInfo(void)
{
	uint32_t data;

	data = PAx5::sMemEEPROM.Read32(EEPROM_LOC_ConfigInfo);

	data = data >> 8;
	configInfo.address = (uint8_t)(data & 0x000000FF);
	data = data >> 8;
	configInfo.verLo   = (uint8_t)(data & 0x000000FF);
	data = data >> 8;
	configInfo.verHi   = (uint8_t)(data & 0x000000FF);
}

bool GW_Config::WriteConfigInfo(void)
{
	uint32_t data;

	data = configInfo.verHi;
	data = data << 8;
	data += configInfo.verLo;
	data = data << 8;
	data += configInfo.address;
	data = data << 8;

	if(!PAx5::sMemEEPROM.Unlock()) return false;
	if(!PAx5::sMemEEPROM.Write32(EEPROM_LOC_ConfigInfo, data)){
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

void GW_Config::ReadKey(uint32_t* key)
{
	if(key == NULL) return;

	uint32_t addr = EEPROM_LOC_NetKeyStart;
	for(uint8_t i = 0; i < 4; i++){
		key[i] = PAx5::sMemEEPROM.Read32(addr);
		addr += EEPROM_ADDR_INC;
	}
}

bool GW_Config::WriteKey(uint32_t* key)
{
	if(key == NULL) return false;

	if(!PAx5::sMemEEPROM.Unlock()) return false;

	uint32_t addr = EEPROM_LOC_NetKeyStart;
	for(uint8_t i = 0; i < 4; i++){
		if(!PAx5::sMemEEPROM.Write32(addr, key[i])){
			PAx5::sMemEEPROM.Lock();
			return false;
		}
		addr += EEPROM_ADDR_INC;
	}

	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

uint32_t GW_Config::ReadFrequency(void)
{
	return PAx5::sMemEEPROM.Read32(EEPROM_LOC_Frequency);
}

bool GW_Config::WriteFrequency(uint32_t freq)
{
	if(!PAx5::sMemEEPROM.Unlock()) return false;
	if(!PAx5::sMemEEPROM.Write32(EEPROM_LOC_Frequency, freq)){
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------
} /* namespace */
