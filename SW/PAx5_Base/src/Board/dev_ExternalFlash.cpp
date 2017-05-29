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

#include "dev_ExternalFlash.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

DEV_ExternalFlash sExternalFlash;

// -----------------------------------------------------------------------------

/* M25P10-A Memory organization
 * 128 KBytes
 * 4 sectors (256 Kbits, 32768 bytes each)
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

// -----------------------------------------------------------------------------

DEV_ExternalFlash::DEV_ExternalFlash() {
	addrH = addrM = addrL = 0;
	dataLen = 0;
}

// -----------------------------------------------------------------------------

void DEV_ExternalFlash::ReadElectronicSignature(void)
{
	dataBuf[0] = M25P_WakeFromDPD;
	dataBuf[1] = 0x00;
	dataBuf[2] = 0x00;
	dataBuf[3] = 0x00;
	dataBuf[4] = 0x00;
	sSPI.SendBufferAndWait(dataBuf, 5, SPISlave_Memory);
}

void DEV_ExternalFlash::ReadIdentification(void)
{
	dataBuf[0] = M25P_ReadID;
	dataBuf[1] = 0x00;
	dataBuf[2] = 0x00;
	dataBuf[3] = 0x00;
	sSPI.SendBufferAndWait(dataBuf, 4, SPISlave_Memory);
}

// -----------------------------------------------------------------------------

bool DEV_ExternalFlash::Initialize(void)
{
	ReadElectronicSignature();
	if(dataBuf[4] != 0x10) return false;

	ReadIdentification();
	if(dataBuf[1] != 0x20) return false;
	if(dataBuf[2] != 0x20) return false;
	if(dataBuf[3] != 0x11) return false;

	return true;
}

void DEV_ExternalFlash::PowerDown(void)
{
	sSPI.SendCommandAndWait(M25P_DeepPowerDown, SPISlave_Memory);
}

void DEV_ExternalFlash::ReleaseFromPD(void)
{
	ReadElectronicSignature();
}

// -----------------------------------------------------------------------------

uint16_t DEV_ExternalFlash::GetPageSize(void)
{
	return M25P_PageSize;
}

uint8_t DEV_ExternalFlash::ReadStatusRegister(void)
{
	return sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
}

bool DEV_ExternalFlash::SetMemoryProtection(EXT_FLASH_ProtectionLevel protectionLevel)
{
	uint8_t regVal, statusReg;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	switch(protectionLevel){
		case EXT_FLASH_ProtectionLevel::M25P_ProtectNone:         regVal = 0x00; break;
		case EXT_FLASH_ProtectionLevel::M25P_ProtectUpperQuarter: regVal = 0x84; break;
		case EXT_FLASH_ProtectionLevel::M25P_ProtectUpperHalf:    regVal = 0x88; break;
		case EXT_FLASH_ProtectionLevel::M25P_ProtectAll:          regVal = 0x8C; break;
		default: return false;
	}

	sSPI.SendCommandAndWait(M25P_WREN, SPISlave_Memory);
	sSPI.SendCmdPlusAndWait(M25P_WriteStatusReg, regVal, SPISlave_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	return (statusReg == regVal);
}

void DEV_ExternalFlash::BulkErase(void)
{
	uint8_t statusReg;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPISlave_Memory);
	sSPI.SendCommandAndWait(M25P_BulkErase, SPISlave_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);
}

void DEV_ExternalFlash::SetAddress(uint32_t addrIn)
{
	addrL = (uint8_t)( addrIn        & 0xFF);
	addrM = (uint8_t)((addrIn >>  8) & 0xFF);
	addrH = (uint8_t)((addrIn >> 16) & 0xFF);
}
uint32_t DEV_ExternalFlash::GetSector()
{
	uint32_t addr;
	addr  = addrH; addr = addr << 8;
	addr |= addrM; addr = addr << 8;
	addr |= addrL;
	addr /= M25P_SectSize;
	return addr;
}
uint32_t DEV_ExternalFlash::GetPageNumber()
{
	uint32_t addr;
	addr  = addrH; addr = addr << 8;
	addr |= addrM; addr = addr << 8;
	addr |= addrL;
	addr /= (uint32_t)M25P_PageSize;
	return addr;
}
uint32_t DEV_ExternalFlash::GetAddress()
{
	uint32_t addr;
	addr  = addrH; addr = addr << 8;
	addr |= addrM; addr = addr << 8;
	addr |= addrL;
	return addr;
}

void DEV_ExternalFlash::SectorErase(void)
{
	uint8_t statusReg;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPISlave_Memory);

	dataBuf[0] = M25P_SectorErase;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, 4, SPISlave_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);
}

void DEV_ExternalFlash::ReadData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	dataBuf[0] = M25P_READ;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, cnt + 4, SPISlave_Memory);
	dataLen = cnt;
}

void DEV_ExternalFlash::WriteData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPISlave_Memory);

	dataBuf[0] = M25P_PageProgram;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, cnt + 4, SPISlave_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);
}

__RAMFUNC void DEV_ExternalFlash::RAM_ReadData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.RAM_SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPISlave_Memory);
	} while((statusReg & 1) != 0);

	dataBuf[0] = M25P_READ;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.RAM_SendBufferAndWait(dataBuf, cnt + 4, SPISlave_Memory);
	dataLen = cnt;
}

__RAMFUNC void DEV_ExternalFlash::RAM_SetAddress(uint32_t addrIn)
{
	addrL = (uint8_t)( addrIn        & 0xFF);
	addrM = (uint8_t)((addrIn >>  8) & 0xFF);
	addrH = (uint8_t)((addrIn >> 16) & 0xFF);
}
// -----------------------------------------------------------------------------
}
