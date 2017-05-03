/**
 * created 2016.02.21 by Calin Radoni
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
