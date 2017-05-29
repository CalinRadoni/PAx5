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

#ifndef dev_EXTERNALFLASH_H_
#define dev_EXTERNALFLASH_H_

#include "cpu_SPI1.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

// TODO ExtFlash: Rebuild this class. Use inheritance and virtual functions!

// -----------------------------------------------------------------------------

enum class EXT_FLASH_ProtectionLevel : uint8_t {
	M25P_ProtectNone,
	M25P_ProtectUpperQuarter,
	M25P_ProtectUpperHalf,
	M25P_ProtectAll
};

const uint32_t M25P_SectSize = 32768U;
const uint16_t M25P_PageSize = 256U;


/** no other object should be interested in these ! */
#define EXT_FLASH_SECTOR_SIZE  M25P_SectSize
#define EXT_FLASH_SECTOR_CNT   4
#define EXT_FLASH_PAGE_SIZE    M25P_PageSize
#define EXT_FLASH_SIZE        (EXT_FLASH_SECTOR_SIZE * EXT_FLASH_SECTOR_CNT)

const     uint16_t EXT_FLASH_BufDataOffset = 4U;
constexpr uint16_t EXT_FLASH_BUFFER_LEN    = (EXT_FLASH_PAGE_SIZE + EXT_FLASH_BufDataOffset);

// -----------------------------------------------------------------------------

/**
 * This class MUST be rewritten !
 * It is ugly !
 *
 * OTA MUST use a high level abstraction not behave like tied to this memory !
 *
 * Also, another memory will have another behavior / requirements so rebuild this
 * with inheritance and virtual functions !
 */
class DEV_ExternalFlash {
public:
	DEV_ExternalFlash();

	uint8_t  dataBuf[EXT_FLASH_BUFFER_LEN];
	uint16_t dataLen;

	bool Initialize(void);

	void PowerDown(void);
	void ReleaseFromPD(void);
	void ReadElectronicSignature(void);
	void ReadIdentification(void);

	uint16_t GetPageSize(void);

	bool SetMemoryProtection(EXT_FLASH_ProtectionLevel);
	void BulkErase(void);
	void SetAddress(uint32_t);
	uint32_t GetSector();
	uint32_t GetPageNumber();
	uint32_t GetAddress();
	void SectorErase(void);
	void ReadData(void);	// length to read is in 'dataLen'
	void WriteData(void);	// length to write is in 'dataLen'

	__RAMFUNC void RAM_ReadData(void); // length to read is in 'dataLen'
	__RAMFUNC void RAM_SetAddress(uint32_t);

protected:
	uint8_t ReadStatusRegister(void);
	uint8_t addrH, addrM, addrL;
};

extern DEV_ExternalFlash sExternalFlash;

}
#endif
