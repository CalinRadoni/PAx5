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

#ifndef CPU_EntropyADC_H_
#define CPU_EntropyADC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t EntropyBufferLenADC = 8;

class CPU_EntropyADC {
public:
	CPU_EntropyADC();
	virtual ~CPU_EntropyADC();

	volatile uint8_t buffer[EntropyBufferLenADC];

	void Enable(void);
	void CollectStart(void);
	void CollectStop(void);
	void Disable(void);

	bool EntropyCollected(void);

	void Collect(void);

	void HandleADCInterrupt(void);

protected:
	void Initialize(void);

	volatile bool     collected;
	volatile uint8_t  buffIdx, buffMask;
	volatile uint8_t  whVal, whCnt;
	volatile uint8_t  entropyByte;
};

extern CPU_EntropyADC entropyADC;

} /* namespace */

#endif
