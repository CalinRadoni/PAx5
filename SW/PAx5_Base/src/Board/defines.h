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

#ifndef SRC_BOARD_DEFINES_H_
#define SRC_BOARD_DEFINES_H_

#include <stddef.h>
#include "versionBase.h"
#include <stm32l0xx.h>

// -----------------------------------------------------------------------------

/**
 * To use __RAMFUNC, in the linker script file (stm32_flash.ld) add the section RamFunctions
 * In .data (Initialized data sections) add:
 *      . = ALIGN(4);
 *      *(.RamFunctions)
 * 	after:
 *      *(.data)
 *      *(.data*)
 *	declarations, and use it like this in function declarations and definitions of functions:
 *      __RAMFUNC void some_ram_function(void);
 */
#define __RAMFUNC __attribute__((section(".RamFunctions")))

// -----------------------------------------------------------------------------

#endif /* SRC_BOARD_DEFINES_H_ */
