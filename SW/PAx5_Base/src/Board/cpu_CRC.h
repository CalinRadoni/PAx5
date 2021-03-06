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

#ifndef CPU_CRC_H_
#define CPU_CRC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_CRC {
public:
	CPU_CRC();
	~CPU_CRC();

	void Enable(void);
	void Disable(void);

	void AddData32(uint32_t);
	void AddData8(uint8_t);
	uint32_t GetData(void);
};

extern CPU_CRC sCRC;

} /* namespace */

#endif
