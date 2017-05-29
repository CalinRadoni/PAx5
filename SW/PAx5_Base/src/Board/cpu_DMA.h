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

#ifndef cpu_DMA_H_
#define cpu_DMA_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_DMA {
public:
	CPU_DMA();

	// DMA1 channel 1 is used for DMA memory-to-memory transfer

	void M2M_Initialize(void);
	void M2M_Copy(uint32_t srcAddr, uint32_t dstAddr, uint32_t dataLen);
	void M2M_CleanUp(void);

	/** Check if the transfer is done
	 *
	 * @return 0 for transfer in progress or not done
	 * @return 1 for transfer completed
	 * @return 2 for transfer error
	 */
	uint8_t M2M_TransferStatus(void);
};

extern CPU_DMA sDMA;

} /* namespace */

#endif
