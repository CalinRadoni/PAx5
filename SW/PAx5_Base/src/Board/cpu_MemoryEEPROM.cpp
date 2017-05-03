/**
 * created 2016.06.16 by Calin Radoni
 */

#include "cpu_MemoryEEPROM.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

FILEID(8)
#define LOG_CODE_Unlock  1 // data unused
#define LOG_CODE_WriteUA 2 // data = userAddress
#define LOG_CODE_WriteSR 3 // data = (uint16_t)(FLASH->SR >> 2)

// -----------------------------------------------------------------------------

#ifdef STM32L051xx
	const uint32_t FLASH_PEKEY1 = 0x89ABCDEFU;
	const uint32_t FLASH_PEKEY2 = 0x02030405U;

	const uint32_t EEPROM_BaseAddress = 0x08080000U;
	const uint32_t EEPROM_Size = 2048U;
#else
	#error "Define FLASH_PEKEYx, EEPROM_BaseAddress and EEPROM_Size for your processor !"
#endif

CPU_MemoryEEPROM sMemEEPROM;

// -----------------------------------------------------------------------------

CPU_MemoryEEPROM::CPU_MemoryEEPROM()
{
}

// -----------------------------------------------------------------------------

bool CPU_MemoryEEPROM::Unlock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// unlock EEPROM if locked
	if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
		FLASH->PEKEYR = FLASH_PEKEY1;
		FLASH->PEKEYR = FLASH_PEKEY2;
		if((FLASH->PECR & FLASH_PECR_PELOCK) != 0){
			hwLogger.AddEntry(FileID, LOG_CODE_Unlock, 0);
			return false;
		}
	}

	return true;
}

void CPU_MemoryEEPROM::Lock(void)
{
	// wait for any operation in progress to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }

	// lock the EEPROM, FLASH will be automatically locked
	FLASH->PECR |= FLASH_PECR_PELOCK;
}

// -----------------------------------------------------------------------------

bool CPU_MemoryEEPROM::Write32(uint32_t userAddr, uint32_t val)
{
	uint32_t address;
	bool res;

	if(userAddr >= EEPROM_Size) return false;

	// no FLASH interrupts, and erase if needed
	FLASH->PECR &= ~(FLASH_PECR_EOPIE | FLASH_PECR_ERRIE | FLASH_PECR_FIX);
	// select data EEPROM
	FLASH->PECR |= FLASH_PECR_DATA;

	// clear some flags
	FLASH->SR = FLASH_SR_FWWER | FLASH_SR_NOTZEROERR | FLASH_SR_RDERR | FLASH_SR_OPTVERR |
				FLASH_SR_SIZERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR | FLASH_SR_EOP;

	// the actual write
	address = EEPROM_BaseAddress + (userAddr & 0xFFFFFFFC);
	*(__IO uint32_t*)address = val;

	// wait for operation to finish
	while((FLASH->SR & FLASH_SR_BSY) != 0){ __NOP(); }
	// check EOP flag
	if((FLASH->SR & FLASH_SR_EOP) != 0){
		FLASH->SR = FLASH_SR_EOP; // clear the EOP flag
		res = true;
	}
	else {
		hwLogger.AddEntry(FileID, LOG_CODE_WriteUA, userAddr);
		// FLASH->SR.BSY and FLAS->SR.EOP are zero here
		hwLogger.AddEntry(FileID, LOG_CODE_WriteSR, (uint16_t)(FLASH->SR >> 2));
		res = false;
	}

	// deselect data EEPROM
	FLASH->PECR &= ~FLASH_PECR_DATA;

	return res;
}

uint32_t CPU_MemoryEEPROM::Read32(uint32_t userAddr)
{
	uint32_t address, val;

	address = EEPROM_BaseAddress + (userAddr & 0xFFFFFFFC);
	val = *(__IO uint32_t*)address;
	return val;
}

// -----------------------------------------------------------------------------
}
