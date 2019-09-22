/**
This file is part of PAx5 (https://github.com/CalinRadoni/PAx5)
Copyright (C) 2016 - 2018 by Calin Radoni

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

#ifndef BASICNETCONFIG_H_
#define BASICNETCONFIG_H_

#include <CommDefines.h>

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

class BasicNetConfig {
public:
	BasicNetConfig();
	virtual ~BasicNetConfig();

	/** \brief Get the version of configuration saved in internal EEPROM */
	uint8_t GetVersion(void);
	/** \brief Set the version of configuration saved in internal EEPROM */
	bool SetVersion(uint8_t);

	/** \brief Write a default configuration in EEPROM */
	bool WriteDefaultConfiguration(void);

	/** \brief Get the 128 bits network key */
	void     GetNetworkKey(uint32_t*);
	/** \brief Set the 128 bits network key */
	bool     SetNetworkKey(uint32_t*);

	uint8_t  GetNodeAddress(void);
	bool     SetNodeAddress(uint8_t);

	uint32_t GetFrequency(void);
	bool     SetFrequency(uint32_t);

	int8_t   GetPower(void);
	bool     SetPower(int8_t);
};

} /* namespace */
#endif
