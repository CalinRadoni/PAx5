/**
 * created 2016.02.21 by Calin Radoni
 */

#ifndef dev_EXTERNALFLASH_H_
#define dev_EXTERNALFLASH_H_

#include "cpu_SPI1.h"

namespace PAx5 {

/* M25P10-A Memory organization
 * 128 kbytes
 * 4 sectors (256 kbits, 32768 bytes each)
 * 512 pages (256 bytes each)
 * Each page can be individually programmed (bits are programmed from 1 to 0)
 * The device is sector or bulk erasable (bits are erased from 0 to 1)
 * Sector 0: 0.00.00h - 07FFFh
 * Sector 1: 0.80.00h - 0FFFFh
 * Sector 2: 1.00.00h - 17FFFh
 * Sector 3: 1.80.00h - 1FFFFh
 *
 * WREN instruction must be called before every PageProgram, SectorErase, BulkErase and WriteStatusReg
 */

#define M25P_ProtectNone         0
#define M25P_ProtectUpperQuarter 1
#define M25P_ProtectUpperHalf    2
#define M25P_ProtectAll          3

#define M25P_WREN           0x06 // no data
#define M25P_WRDI           0x04 // no data
#define M25P_ReadID         0x9F // 1 - 3 data bytes
#define M25P_ReadStatusReg  0x05 // 1 data byte minimum
#define M25P_WriteStatusReg 0x01 // 1 data byte
#define M25P_READ           0x03 // 3 address + minimum 1 data byte
#define M25P_FAST_READ      0x0B // 3 address + 1 dummy + minimum 1 data byte
#define M25P_PageProgram    0x02 // 3 address + 1 - 256 data bytes
#define M25P_SectorErase    0xD8 // 3 address
#define M25P_BulkErase      0xC7 // no data
#define M25P_DeepPowerDown  0xB9 // no data
#define M25P_WakeFromDPD    0xAB // 3 dummy + minimum 1 data byte

#define M25P_SectSize 32768
#define M25P_PageSize 256

// -----

#define EXT_FLASH_SECTOR_SIZE  M25P_SectSize
#define EXT_FLASH_SECTOR_CNT   4
#define EXT_FLASH_PAGE_SIZE    M25P_PageSize
#define EXT_FLASH_SIZE        (EXT_FLASH_SECTOR_SIZE * EXT_FLASH_SECTOR_CNT)

#define EXT_FLASH_BufDataOffset  4
#define EXT_FLASH_BUFFER_LEN    (EXT_FLASH_PAGE_SIZE + EXT_FLASH_BufDataOffset)

// -----

class DEV_ExternalFlash {
protected:
	uint8_t ReadStatusRegister(void);
	uint8_t addrH, addrM, addrL;

public:
	DEV_ExternalFlash();

	uint8_t  dataBuf[EXT_FLASH_BUFFER_LEN];
	uint16_t dataLen;

	bool Initialize(void);

	void PowerDown(void);
	void ReleaseFromPD(void);
	void ReadElectronicSignature(void);
	void ReadIdentification(void);

	bool SetMemoryProtection(uint8_t);
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
};

extern DEV_ExternalFlash sExternalFlash;

}
#endif
