/**
 * created 2016.05.29 by Calin Radoni
 */

#include "cpu_MemoryFlash.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(9)
#define LOG_CODE_Unlock  1 // data {1 = EEPROM unlock failed, 2 = FLASH unlock failed, 3 = Option bytes unlock failed }
#define LOG_CODE_EraseUA 2 // data = page address
#define LOG_CODE_EraseSR 3 // data = (uint16_t)(FLASH->SR >> 2)
#define LOG_CODE_WriteUA 4 // data = half-page address
#define LOG_CODE_WriteSR 5 // data = (uint16_t)(FLASH->SR >> 2)

// -----------------------------------------------------------------------------

#ifdef STM32L051xx
	const uint32_t FLASH_PEKEY1  = 0x89ABCDEFU;
	const uint32_t FLASH_PEKEY2  = 0x02030405U;
	const uint32_t FLASH_PRGKEY1 = 0x8C9DAEBFU;
	const uint32_t FLASH_PRGKEY2 = 0x13141516U;
	const uint32_t FLASH_OPTKEY1 = 0xFBEAD9C8U;
	const uint32_t FLASH_OPTKEY2 = 0x24252627U;
#else
	#error "Define FLASH_PEKEY and FLASH_PRGKEY for your processor !"
#endif

uint32_t CPU_MemoryFLASH_address;

// -----------------------------------------------------------------------------

__RAMFUNC bool CPU_MemoryFLASH_Unlock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// unlock EEPROM if locked
	if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
		if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 1);
			return false;
		}
	}

	// unlock FLASH if locked
	if((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0){
		FLASH->PRGKEYR = FLASH_PRGKEY1;
		FLASH->PRGKEYR = FLASH_PRGKEY2;
		if((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 2);
			return false;
		}
	}

	return true;
}

__RAMFUNC void CPU_MemoryFLASH_Lock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// lock the EEPROM, FLASH will be automatically locked
	FLASH->PECR |= FLASH_PECR_PELOCK;
}

// -----------------------------------------------------------------------------

__RAMFUNC bool CPU_MemoryFLASH_ErasePage(void)
{
	bool res;

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	// enable erasing
	FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_PROG;
	// erasing starts by writing a 32-bit word value in the page's address
	*(__IO uint32_t *)CPU_MemoryFLASH_address = (uint32_t)0;
	// wait for operation to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }
	// check EOP flag
	if((FLASH->SR & FLASH_SR_EOP) != 0){
		FLASH->SR = FLASH_SR_EOP; // clear the EOP flag
		res = true;
	}
	else{
		// log page number
		hwLogger.AddEntry(FileID, LOG_CODE_EraseUA, (uint16_t)(CPU_MemoryFLASH_address >> 7));
		// FLASH->SR.BSY and FLAS->SR.EOP are zero here
		hwLogger.AddEntry(FileID, LOG_CODE_EraseSR, (uint16_t)(FLASH->SR >> 2));
		res = false;
	}
	// disable erasing
	FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_PROG);

	if(pMask == 0) __enable_irq();

	return res;
}

// -----------------------------------------------------------------------------

__RAMFUNC bool CPU_MemoryFLASH_WriteHalfPage(uint8_t *data)
{
	bool res;
	uint32_t dw;

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	// clear some flags
	FLASH->SR = FLASH_SR_FWWER | FLASH_SR_NOTZEROERR | FLASH_SR_RDERR | FLASH_SR_OPTVERR |
				FLASH_SR_SIZERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR | FLASH_SR_EOP;

	// enable half page programming
	FLASH->PECR |= FLASH_PECR_PROG | FLASH_PECR_FPRG;

	// copy data to be written
	for(uint8_t i = 0; i < CPUMF_HALFPAGE_SIZE_DW; i++){
		dw  = *data++; dw = dw << 8;
		dw |= *data++; dw = dw << 8;
		dw |= *data++; dw = dw << 8;
		dw |= *data++;
		*(__IO uint32_t*)CPU_MemoryFLASH_address = dw;
		CPU_MemoryFLASH_address += 4;
	}
	// wait for operation to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }
	// check EOP flag
	if((FLASH->SR & FLASH_SR_EOP) != 0){
		FLASH->SR = FLASH_SR_EOP; // clear the EOP flag
		res = true;
	}
	else {
		// log half-page number
		hwLogger.AddEntry(FileID, LOG_CODE_WriteUA, (uint16_t)(CPU_MemoryFLASH_address >> 6));
		// FLASH->SR.BSY and FLAS->SR.EOP are zero here
		hwLogger.AddEntry(FileID, LOG_CODE_WriteSR, (uint16_t)(FLASH->SR >> 2));
		res = false;
	}

	// disable half page programming
	FLASH->PECR &= ~(FLASH_PECR_PROG | FLASH_PECR_FPRG);

	if(pMask == 0) __enable_irq();

	return res;
}

// -----------------------------------------------------------------------------

__RAMFUNC bool CPU_OptionBytes_Unlock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// unlock EEPROM if locked
	if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
		if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 1);
			return false;
		}
	}

	// unlock FLASH if locked
	if((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0){
		FLASH->PRGKEYR = FLASH_PRGKEY1;
		FLASH->PRGKEYR = FLASH_PRGKEY2;
		if((FLASH->PECR & FLASH_PECR_PRGLOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 2);
			return false;
		}
	}

	// unlock Option bytes area if locked
	if((FLASH->PECR & FLASH_PECR_OPTLOCK) != 0){
		FLASH->OPTKEYR = FLASH_OPTKEY1;
		FLASH->OPTKEYR = FLASH_OPTKEY2;
		if((FLASH->PECR & FLASH_PECR_OPTLOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 3);
			return false;
		}
	}

	return true;
}

__RAMFUNC void CPU_OptionBytes_Lock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// lock the EEPROM, FLASH and Option bytes will be automatically locked
	FLASH->PECR |= FLASH_PECR_PELOCK;
}

// -----------------------------------------------------------------------------
}
