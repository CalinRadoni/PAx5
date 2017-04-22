/**
 * created 2016.02.21 by Calin Radoni
 */

// -----------------------------------------------------------------------------

#include "dev_ExternalFlash.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

DEV_ExternalFlash sExternalFlash;

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
	sSPI.SendBufferAndWait(dataBuf, 5, SPI_SLAVE_Memory);
}

void DEV_ExternalFlash::ReadIdentification(void)
{
	dataBuf[0] = M25P_ReadID;
	dataBuf[1] = 0x00;
	dataBuf[2] = 0x00;
	dataBuf[3] = 0x00;
	sSPI.SendBufferAndWait(dataBuf, 4, SPI_SLAVE_Memory);
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
	sSPI.SendCommandAndWait(M25P_DeepPowerDown, SPI_SLAVE_Memory);
}

void DEV_ExternalFlash::ReleaseFromPD(void)
{
	ReadElectronicSignature();
}

// -----------------------------------------------------------------------------

uint8_t DEV_ExternalFlash::ReadStatusRegister(void)
{
	return sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
}

bool DEV_ExternalFlash::SetMemoryProtection(uint8_t protectionLevel)
{
	uint8_t regVal, statusReg;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	switch(protectionLevel){
		case M25P_ProtectNone:         regVal = 0x00; break;
		case M25P_ProtectUpperQuarter: regVal = 0x84; break;
		case M25P_ProtectUpperHalf:    regVal = 0x88; break;
		case M25P_ProtectAll:          regVal = 0x8C; break;
		default: return false;
	}

	sSPI.SendCommandAndWait(M25P_WREN, SPI_SLAVE_Memory);
	sSPI.SendCmdPlusAndWait(M25P_WriteStatusReg, regVal, SPI_SLAVE_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	return (statusReg == regVal);
}

void DEV_ExternalFlash::BulkErase(void)
{
	uint8_t statusReg;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPI_SLAVE_Memory);
	sSPI.SendCommandAndWait(M25P_BulkErase, SPI_SLAVE_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
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
	addr /= M25P_PageSize;
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
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPI_SLAVE_Memory);

	dataBuf[0] = M25P_SectorErase;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, 4, SPI_SLAVE_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);
}

void DEV_ExternalFlash::ReadData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	dataBuf[0] = M25P_READ;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, cnt + 4, SPI_SLAVE_Memory);
	dataLen = cnt;
}

void DEV_ExternalFlash::WriteData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	sSPI.SendCommandAndWait(M25P_WREN, SPI_SLAVE_Memory);

	dataBuf[0] = M25P_PageProgram;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.SendBufferAndWait(dataBuf, cnt + 4, SPI_SLAVE_Memory);
	do{
		statusReg = sSPI.SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);
}

__RAMFUNC void DEV_ExternalFlash::RAM_ReadData(void)
{
	uint16_t cnt;
	uint8_t statusReg;

	if(dataLen > M25P_PageSize) cnt = M25P_PageSize;
	else                        cnt = dataLen;

	do{
		statusReg = sSPI.RAM_SendCmdPlusAndWait(M25P_ReadStatusReg, 0, SPI_SLAVE_Memory);
	} while((statusReg & 1) != 0);

	dataBuf[0] = M25P_READ;
	dataBuf[1] = addrH;
	dataBuf[2] = addrM;
	dataBuf[3] = addrL;
	sSPI.RAM_SendBufferAndWait(dataBuf, cnt + 4, SPI_SLAVE_Memory);
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
