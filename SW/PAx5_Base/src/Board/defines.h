/**
 * created 2016.MM.DD by Calin Radoni
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
