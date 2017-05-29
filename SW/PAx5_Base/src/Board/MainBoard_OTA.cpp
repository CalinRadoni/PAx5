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

#include "MainBoard_OTA.h"
#include "dev_ExternalFlash.h"
#include "cpu_MemoryFlash.h"
#include "cpu_Info.h"
#include "cpu_CRC.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

uint32_t firmwareAddressInExternalFLASH = 0;
uint32_t internalFlashSize = 0;

// -----------------------------------------------------------------------------

#define RAM_WF_LED_ON()  { GPIOA->BSRR = GPIO_BSRR_BR_15; }
#define RAM_WF_LED_OFF() { GPIOA->BSRR = GPIO_BSRR_BS_15; }

// somewhere around 510-530 ms and 190-200 ms
#define WriteNewFirmware_LONG  0x001E0000
#define WriteNewFirmware_SHORT 0x000B0000

/** Write firmware from external FLASH to the internal FLASH
 *
 * If write conditions are not OK the function will return false.
 *
 * After writing begins the function will restart the CPU, even if the firmware update failed.
 * Before restart the status should be visible on the onboard LED.
 */
__RAMFUNC bool WriteNewFirmware(uint32_t firmwareCodeLength)
{
	if(firmwareCodeLength == 0) return false;

	if(firmwareCodeLength > internalFlashSize) return false;
	uint32_t flashSize = firmwareCodeLength;

	if(!sExternalFlash.Initialize()) return false;

	if(!sSPI.ConfigureForRAM()) return false;

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	if(!CPU_MemoryFLASH_Unlock()){
		if(pMask == 0) __enable_irq();

		sSPI.Configure();
		return false;
	}

	// from here on the interrupts are disabled and CPU will be reset at end

	static_assert(CPUMF_PAGE_SIZE_Bytes <= EXT_FLASH_PAGE_SIZE, "wrong page size");

	RAM_WF_LED_ON();

	bool res = true;
	uint32_t extFlashAddr = firmwareAddressInExternalFLASH;
	uint32_t flashAddr    = CPU_MemoryFLASH_StartAddress;
	while((flashSize > 0) && res){
		// read from external FLASH
		sExternalFlash.dataLen = CPUMF_PAGE_SIZE_Bytes;
		sExternalFlash.RAM_SetAddress(extFlashAddr);
		sExternalFlash.RAM_ReadData();
		extFlashAddr += CPUMF_PAGE_SIZE_Bytes;

		// erase FLASH page
		CPU_MemoryFLASH_address = flashAddr;
		res &= CPU_MemoryFLASH_ErasePage();

		// write first half page of internal FLASH
		CPU_MemoryFLASH_address = flashAddr;
		res &= CPU_MemoryFLASH_WriteHalfPage(&sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset]);
		flashAddr += CPUMF_HALFPAGE_SIZE_Bytes;

		if(flashSize > CPUMF_HALFPAGE_SIZE_Bytes){
			// write second half page of internal FLASH
			CPU_MemoryFLASH_address = flashAddr;
			res &= CPU_MemoryFLASH_WriteHalfPage(&sExternalFlash.dataBuf[EXT_FLASH_BufDataOffset + CPUMF_HALFPAGE_SIZE_Bytes]);
			flashAddr += CPUMF_HALFPAGE_SIZE_Bytes;
		}

		if(flashSize > CPUMF_PAGE_SIZE_Bytes) flashSize -= CPUMF_PAGE_SIZE_Bytes;
		else                                  flashSize = 0;
	}

	RAM_WF_LED_OFF();

	volatile uint32_t www;
	uint8_t i, j;

	www = WriteNewFirmware_SHORT; while(www--);

	j = res ? 5 : 20;
	for(i = 0; i < j; i++){
		RAM_WF_LED_ON();  www = WriteNewFirmware_SHORT; while(www--);
		RAM_WF_LED_OFF(); www = WriteNewFirmware_SHORT; while(www--);
		if(res){
			RAM_WF_LED_ON();  www = WriteNewFirmware_LONG; while(www--);
			RAM_WF_LED_OFF(); www = WriteNewFirmware_SHORT; while(www--);
		}
	}

#ifdef STM32L051xx
	// From ARM Cortex-M0+ Devices - Generic User Guide (ARM DUI 0662B (ID011713)
	// This is the procedure for ARM Cortex-M0+ devices
	SCB->AIRCR = ((0x05FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
#else
	#error Define Software reset procedure for your processor if is not ARM Cortex-M0+ !
#endif

	for(;;) __NOP(); // wait for reset to take place

	return true;
}

// -----------------------------------------------------------------------------

MainBoardOTA::MainBoardOTA()
{
	InternalInit();
}

// -----------------------------------------------------------------------------

void MainBoardOTA::InternalInit(void)
{
	internalFlashSize = sCPU.GetFlashMemSize();  ///< memory size in Kbytes
	internalFlashSize = internalFlashSize << 10; ///< memory size in bytes

	uint32_t var = internalFlashSize;
	var /= EXT_FLASH_SECTOR_SIZE;                    ///< required sectors in external FLASH
	if(var > EXT_FLASH_SECTOR_CNT){
		currentFlashAddr = 0;
		firmwareAddressInExternalFLASH = 0;
		return;
	}

	var = EXT_FLASH_SECTOR_CNT - var;				 ///< from this sector the firmware should be stored
	firmwareAddressInExternalFLASH = var;
	firmwareAddressInExternalFLASH *= EXT_FLASH_SECTOR_SIZE;
	currentFlashAddr = firmwareAddressInExternalFLASH;

	flashBuffIdx = EXT_FLASH_BufDataOffset;
	firmwareSize = 0;
}

void MainBoardOTA::InitDataAndExtFLASH(void)
{
	uint32_t var = firmwareAddressInExternalFLASH;

	// erase some sectors in external flash
	while(var < EXT_FLASH_SIZE){
		sExternalFlash.SetAddress(var);
		sExternalFlash.SectorErase();
		var += EXT_FLASH_SECTOR_SIZE;
	}

	// set variables to begin adding data
	currentFlashAddr = firmwareAddressInExternalFLASH;
	flashBuffIdx = EXT_FLASH_BufDataOffset;
	firmwareSize = 0;
}

bool MainBoardOTA::AddData(uint8_t *buffer, uint8_t len)
{
	for(uint8_t i = 0; i < len; i++){
		sExternalFlash.dataBuf[flashBuffIdx] = buffer[i];
		flashBuffIdx++;
		if(flashBuffIdx >= EXT_FLASH_BUFFER_LEN){
			// ext.flash buffer full

			// this should not happen !
			if(currentFlashAddr >= EXT_FLASH_SIZE) return false;

			// write the buffer
			sExternalFlash.SetAddress(currentFlashAddr);
			sExternalFlash.dataLen = EXT_FLASH_PAGE_SIZE;
			sExternalFlash.WriteData();

			// set the buffer for next data
			flashBuffIdx = EXT_FLASH_BufDataOffset;
			currentFlashAddr += EXT_FLASH_PAGE_SIZE;
		}
	}
	return true;
}

bool MainBoardOTA::Finalize(void)
{
	// write the last buffer if it si not empty

	if(flashBuffIdx <= EXT_FLASH_BufDataOffset){
		firmwareSize = currentFlashAddr - firmwareAddressInExternalFLASH;
		return true;
	}
	if(currentFlashAddr >= EXT_FLASH_SIZE){
		firmwareSize = 0;
		return false;
	}

	while(flashBuffIdx < EXT_FLASH_BUFFER_LEN){
		sExternalFlash.dataBuf[flashBuffIdx] = 0xFF;
		flashBuffIdx++;
	}

	sExternalFlash.SetAddress(currentFlashAddr);
	sExternalFlash.dataLen = EXT_FLASH_PAGE_SIZE;
	sExternalFlash.WriteData();

	// compute firmware size

	flashBuffIdx = EXT_FLASH_BufDataOffset;
	currentFlashAddr += EXT_FLASH_PAGE_SIZE;
	firmwareSize = currentFlashAddr - firmwareAddressInExternalFLASH;

	return true;
}

// -----------------------------------------------------------------------------

uint32_t MainBoardOTA::ComputeCRCforStoredFirmware(void)
{
	sCRC.Enable();

	static_assert(CPUMF_PAGE_SIZE_Bytes <= EXT_FLASH_PAGE_SIZE, "wrong page size");

	uint32_t flashSize = firmwareSize;
	uint32_t useSize;
	uint32_t extFlashAddr = firmwareAddressInExternalFLASH;
	while(flashSize > 0){
		// read from external FLASH
		sExternalFlash.dataLen = CPUMF_PAGE_SIZE_Bytes;
		sExternalFlash.SetAddress(extFlashAddr);
		sExternalFlash.RAM_ReadData();
		extFlashAddr += CPUMF_PAGE_SIZE_Bytes;

		if(flashSize > CPUMF_PAGE_SIZE_Bytes) useSize = CPUMF_PAGE_SIZE_Bytes;
		else                                  useSize = flashSize;
		flashSize -= useSize;

		flashBuffIdx = EXT_FLASH_BufDataOffset;
		while(useSize > 0){
			sCRC.AddData8(sExternalFlash.dataBuf[flashBuffIdx]);
			flashBuffIdx++;
			useSize--;
		}
	}

	uint32_t computedCRC = sCRC.GetData();
	sCRC.Disable();

	return computedCRC;
}

bool MainBoardOTA::TransferFirmware(void)
{
	return WriteNewFirmware(firmwareSize);
}

// -----------------------------------------------------------------------------
}
