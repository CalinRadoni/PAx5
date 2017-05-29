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

#ifndef GW_CONFIG_H_
#define GW_CONFIG_H_

#include "defines.h"

namespace PAx5Gateway {

// -----------------------------------------------------------------------------

/** EEPROM Configuration
 *
 *  0: version major | version minor | node address | reserved
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
 *  9: frequency
 * 10: (int8_t)power | unused | unused | unused
 */

// -----------------------------------------------------------------------------

struct BoardConfigInfo
{
	uint8_t verHi;
	uint8_t verLo;
	uint8_t address;
	uint8_t reserved;
};

class GW_Config
{
public:
	GW_Config();

	BoardConfigInfo configInfo;
	void ReadConfigInfo(void);
	bool WriteConfigInfo(void);

	void ReadKey(uint32_t*);
	bool WriteKey(uint32_t*);

	uint32_t ReadFrequency(void);
	bool WriteFrequency(uint32_t);
};

} /* namespace */

#endif
