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

/**
 * This file declare the class CPU_MemoryEEPROM useable for working with CPU's internal EEPROM
 *
 * Memory endianness: little-endian, LSB of a word is stored at the lowest-numbered byte
 * For 32-bit word, address A = LSB, address A+3 = MSB
 *
 * Programming in the EEPROM memory performed by word, half-word or byte
 * Erase operation performed by page (in Flash memory, data EEPROM and Option bytes)
 * Mass erase operation
 *
 * The NVM is organized as 32-bit memory cells
 * The memory array is divided into pages.
 * A page is composed of 1 single words (or 4 bytes) in EEPROM memory
 *
 * STM32L051x is a Category 3 device
 * Category 3 devices EEPROM memory:
 * 	- physical address 0x0808 0000 - 0x0808 07FF
 * 	- size 2 Kbytes
 */

#ifndef cpu_MEMORYEEPROM_H_
#define cpu_MEMORYEEPROM_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_MemoryEEPROM {
public:
	CPU_MemoryEEPROM();

	/** Unlock the EEPROM to enable writing operations
	 *
	 * PELOCK will be set to zero
	 */
	bool Unlock(void);

	/** Lock the EEPROM to disable writing operations
	 *
	 * PELOCK will be set to one
	 */
	void Lock(void);

	/** Write a 32-bit word to EEPROM
	 *
	 * The EEPROM memory will be erased automatically, if needed, before writing.
	 *
	 * According to the STM32L0x1 Reference Manual RM0377 (DocID025942 Rev 5), 3.3.4 Writing/erasing the NVM, page 76:
	 * the write error WRPERR is generated if PELOCK is 1 (CPU_MemoryEEPROM::Unlock is setting that to zero)
	 * or if the memory is protected.
	 *
	 * @warning userAddress will be aligned down (& 0xFFFFFFFC)!
	 *
	 * @param userAddress The address (0 to 2k for STM32L051K8) to write to. This address SHOULD be aligned.
	 * 		  Inside the function the address WILL BE ALIGNED DOWN !
	 * @param data The 32-bit data to be written.
	 * @return False if destination address is bigger then EEPROM size
	 * @return False if writting error
	 * @return True on success
	 */
	bool Write32(uint32_t, uint32_t);

	/** Read a 32-bit word from EEPROM
	 *
	 * @param userAddress The address (0 to 2k for STM32L051K8) to read from. This address SHOULD be aligned.
	 * @warning userAddress will be aligned down (& 0xFFFFFFFC)!
	 * @return The 32-bit data readed.
	 */
	uint32_t Read32(uint32_t);
};

extern CPU_MemoryEEPROM sMemEEPROM;

} /* namespace */

#endif /* cpu_MEMORYEEPROM_H_ */
