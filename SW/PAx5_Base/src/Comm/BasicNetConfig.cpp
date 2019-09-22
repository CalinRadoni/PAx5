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

#include "BasicNetConfig.h"
#include "cpu_MemoryEEPROM.h"
#include "CommDefines.h"

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

/** EEPROM Configuration, version 1
 *
 *  0: reserved | reserved | reserved | configuration version = 1
 *
 *  1: network key[0]
 *  2: network key[1]
 *  3: network key[2]
 *  4: network key[3]
 *  5: reserved
 *  6: reserved
 *  7: reserved
 *  8: reserved
 *
 *  9: - | - | - | node address
 * 10: frequency
 * 11: - | - | - | (int8_t)power
 */

const uint32_t EEPROM_AddressIncrement = 4;

const uint32_t EEPROM_ADDR_Version     = ( 0 << 2);
const uint32_t EEPROM_ADDR_NetKey      = ( 1 << 2);
const uint32_t EEPROM_ADDR_NodeAddress = ( 9 << 2);
const uint32_t EEPROM_ADDR_Frequency   = (10 << 2);
const uint32_t EEPROM_ADDR_Power       = (11 << 2);

// -----------------------------------------------------------------------------

BasicNetConfig::BasicNetConfig()
{
	//
}

// -----------------------------------------------------------------------------

uint8_t BasicNetConfig::GetVersion(void)
{
	uint32_t data = PAx5::sMemEEPROM.Read32(EEPROM_ADDR_Version);

	return (uint8_t)(data & 0x000000FF);
}

bool BasicNetConfig::SetVersion(uint8_t version)
{
	uint32_t data = version;

	if (!PAx5::sMemEEPROM.Unlock()) return false;
	if (!PAx5::sMemEEPROM.Write32(EEPROM_ADDR_Version, data)) {
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

bool BasicNetConfig::WriteDefaultConfiguration(void)
{
	if (!SetVersion(1))            return false;
	if (!SetNetworkKey(NULL))      return false;
	if (!SetNodeAddress(0xFF))     return false;
	if (!SetFrequency(0xFFFFFFFF)) return false;
	if (!SetPower(0x00))           return false;
	return true;
}

// -----------------------------------------------------------------------------

void BasicNetConfig::GetNetworkKey(uint32_t *key)
{
	if (key == NULL) return;

	uint32_t addr = EEPROM_ADDR_NetKey;
	for (uint8_t i = 0; i < 4; i++) {
		key[i] = PAx5::sMemEEPROM.Read32(addr);
		addr += EEPROM_AddressIncrement;
	}
}

bool BasicNetConfig::SetNetworkKey(uint32_t *key)
{
	if (!PAx5::sMemEEPROM.Unlock()) return false;

	uint32_t addr = EEPROM_ADDR_NetKey;
	uint32_t data;
	for (uint8_t i = 0; i < 4; i++) {
		if (key == NULL) data = 0xFFFFFFFF;
		else             data = key[i];
		if (!PAx5::sMemEEPROM.Write32(addr, data)) {
			PAx5::sMemEEPROM.Lock();
			return false;
		}
		addr += EEPROM_AddressIncrement;
	}

	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

uint8_t BasicNetConfig::GetNodeAddress(void)
{
	uint32_t data = PAx5::sMemEEPROM.Read32(EEPROM_ADDR_NodeAddress);

	return (uint8_t)(data & 0x000000FF);
}

bool BasicNetConfig::SetNodeAddress(uint8_t nodeAddress)
{
	uint32_t data = nodeAddress;

	if (!PAx5::sMemEEPROM.Unlock()) return false;
	if (!PAx5::sMemEEPROM.Write32(EEPROM_ADDR_NodeAddress, data)) {
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

uint32_t BasicNetConfig::GetFrequency(void)
{
	return PAx5::sMemEEPROM.Read32(EEPROM_ADDR_Frequency);
}

bool BasicNetConfig::SetFrequency(uint32_t freq)
{
	if (!PAx5::sMemEEPROM.Unlock()) return false;
	if (!PAx5::sMemEEPROM.Write32(EEPROM_ADDR_Frequency, freq)) {
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------

int8_t BasicNetConfig::GetPower(void)
{
	uint32_t data = PAx5::sMemEEPROM.Read32(EEPROM_ADDR_Power);

	int32_t tmp = (int32_t)(data & 0x000000FF);
	tmp -= 128; // [0 ... 255] -> [-128 ... 127]
	int8_t power = (int8_t)tmp;
	return power;
}

bool BasicNetConfig::SetPower(int8_t power)
{
	int32_t tmp = power;
	tmp += 128; // [-128 ... 127] -> [0 ... 255]

	uint32_t data = tmp;

	if (!PAx5::sMemEEPROM.Unlock()) return false;
	if (!PAx5::sMemEEPROM.Write32(EEPROM_ADDR_Power, data)) {
		PAx5::sMemEEPROM.Lock();
		return false;
	}
	PAx5::sMemEEPROM.Lock();
	return true;
}

// -----------------------------------------------------------------------------
} /* namespace */
