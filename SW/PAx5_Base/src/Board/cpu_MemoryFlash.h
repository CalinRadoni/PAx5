/**
 * created 2016.05.29 by Calin Radoni
 *
 * This file declare the functions useable for working with CPU's internal FLASH
 *
 * Flash Start address: 0x00000000
 * SRAM Start address: 0x20000000
 * Memory endianness: little-endian, LSB of a word is stored at the lowest-numbered byte
 * For 32-bit word, address A = LSB, address A+3 = MSB
 *
 * STM32L051x is in Category 3 (a single bank of Flash)
 * STM32L071x is in Category 5 (two banks of Flash)
 *
 * Programming in the Flash memory performed by word or half-page
 * Erase operation performed by page (in Flash memory, data EEPROM and Option bytes)
 * Mass erase operation
 *
 * The NVM is organized as 32-bit memory cells
 * The memory array is divided into pages.
 * A page is composed of 32 words (or 128 bytes) in Flash program memory and System memory
 *
 * A Flash sector is made of 32 pages (or 4 Kbytes). The sector is the granularity of the write protection.
 *
 * For category 3 devices Flash memory (each page is 128 bytes):
 * Page 0 : 0x0800 0000 - 0x0800 007F
 * ...
 * Page 255: 0x0800 7F80 - 0x0800 7FFF --- 32K devices (8 sectors)
 * ...
 * Page 511: 0x0800 FF80 - 0x0800 FFFF --- 64K devices (16 sectors)
 */

#ifndef cpu_MEMORYFLASH_H_
#define cpu_MEMORYFLASH_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define CPUMF_HALFPAGE_SIZE_DW     16
#define CPUMF_HALFPAGE_SIZE_Bytes (CPUMF_HALFPAGE_SIZE_DW << 2)
#define CPUMF_PAGE_SIZE_Bytes     (CPUMF_HALFPAGE_SIZE_Bytes << 1)

/**
 * @attention Do NOT use a dedicated buffer ! For OTA there is a big buffer available in DEV_ExternalFlash.
 */

/** Address for Erase and WriteHalfPage operations
 *
 * For erase operation the address does not need to be aligned.
 * This address MUST be aligned to a half-page for CPU_MemoryFLASH_WriteHalfPage
 */
extern uint32_t CPU_MemoryFLASH_address;

#define CPU_MemoryFLASH_Sector   ((CPU_MemoryFLASH_address - 0x08000000) >> 12)
#define CPU_MemoryFLASH_Page     ((CPU_MemoryFLASH_address - 0x08000000) >> 7)
#define CPU_MemoryFLASH_HalfPage ((CPU_MemoryFLASH_address >> 6) & 0x01)

#define CPU_MemoryFLASH_StartAddress 0x08000000

/** Unlock the FLASH to enable erasing and writing operations
 *
 * PELOCK and PRGLOCK will be set to zero
 */
__RAMFUNC bool CPU_MemoryFLASH_Unlock(void);

/** Lock the FLASH so erasing and reading will be blocked
 *
 * PELOCK is set to 1 (will automatically set PRGLOCK to one)
 */
__RAMFUNC void CPU_MemoryFLASH_Lock(void);

/** Erase a page in the FLASH Memory
 *
 * According to the STM32L0x1 Reference Manual RM0377 (DocID025942 Rev 5), 3.3.4 Writing/erasing the NVM, page 83
 * the possible write errors are:
 * - WRPERR if PELOCK or PRGLOCK are 1 (CPU_MemoryFLASH_Unlock set those to zero) or if the address is in a write-protected sector;
 * - SIZERR if the size is not the word. The function uses uint32_t so this will not appear;
 *
 * The address should be inside the page that needs to be erased.
 *
 * @warning This function disables interrupts while writting !
 *
 * @example
 * Delete pages 510 and 511
 * @code{.cpp}
 * if(CPU_MemoryFLASH_Unlock()){
 *     CPU_MemoryFLASH_address = 0x0800FF00; // start of page 510
 *     CPU_MemoryFLASH_ErasePage();
 *     CPU_MemoryFLASH_address = 0x0800FF80; // start of page 511
 *     CPU_MemoryFLASH_ErasePage();
 *     CPU_MemoryFLASH_Lock();
 * }
 * @endcode
 */
__RAMFUNC bool CPU_MemoryFLASH_ErasePage(void);

/** Write a half page in the FLASH Memory
 *
 * According to the STM32L0x1 Reference Manual RM0377 (DocID025942 Rev 5), 3.3.4 Writing/erasing the NVM, page 81-82
 * the possible write errors are:
 * - WRPERR if PELOCK or PRGLOCK are 1 (CPU_MemoryFLASH_Unlock set those to zero) or if the address is in a write-protected sector;
 * - NOTZEROERR if address points to a word which is not zero. Cat 3 devices will write but others will abort;
 * - SIZERR if the size is not the word. The function uses uint32_t so this will not appear;
 * - PGAERR if CPU_MemoryFLASH_address is not a start of a half-page;
 * - FWWERR if the master fetched in the NVM. Interrupts are disabled so this will not appear.
 *
 * @warning This address MUST be aligned to a half-page !
 *
 * @warning This function disables interrupts while writting !
 *
 * @example
 * Write page 511
 * @code{.cpp}
 * uint8_t wdata[CPUMF_HALFPAGE_SIZE_Bytes];
 * if(CPU_MemoryFLASH_Unlock()){
 *     CPU_MemoryFLASH_address = 0x0800FF80; // page 511, start of first half-page
 *     CPU_MemoryFLASH_WriteHalfPage(wdata);
 *     CPU_MemoryFLASH_address = 0x0800FFC0; // page 511, start of second half-page
 *     CPU_MemoryFLASH_WriteHalfPage(wdata);
 *     CPU_MemoryFLASH_Lock();
 * }
 * @endcode
 */
__RAMFUNC bool CPU_MemoryFLASH_WriteHalfPage(uint8_t*);

/** Unlock the Option bytes area
 *
 * PELOCK, PRGLOCK and OPTLOCK will be set to zero
 */
__RAMFUNC bool CPU_OptionBytes_Unlock(void);

/** Lock the Option bytes area
 *
 * PELOCK is set to 1 (will automatically set PRGLOCK and OPTLOCK to one)
 */
__RAMFUNC void CPU_OptionBytes_Lock(void);
} /* namespace */

#endif /* cpu_MEMORYFLASH_H_ */
