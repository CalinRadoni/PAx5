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

#include "cpu_Info.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_Info sCPUInfo;

// -----------------------------------------------------------------------------

CPU_Info::CPU_Info()
{
	catID = 0;
	revID = '?';

	uniqueID[0] = 0;
	uniqueID[1] = 0;
	uniqueID[2] = 0;
}

CPU_Info::~CPU_Info() {
}

// -----------------------------------------------------------------------------

void CPU_Info::ReadDeviceID(void)
{
	uint32_t val;
	uint16_t v16;

	//From STM32L051 Reference Manual - RM0377 (DocID 025942 Rev.5), 27.4.1.MCU device ID code

	val = DBGMCU->IDCODE;

	catID = 0;
	revID = '?';

#ifdef STM32L051xx
	v16 = val & 0x00000FFF;
	switch(v16){
		case 0x457: catID = 1; break;
		case 0x425: catID = 2; break;
		case 0x417: catID = 3; break;
		case 0x447: catID = 5; break;
	}

	v16 = val >> 16;
	switch(v16){
		case 0x1000: revID = 'A'; break;
		case 0x1008:
			if((catID==1) || (catID==3)) revID = 'Z';
			break;
		case 0x1018:
			if(catID==3) revID = 'Y';
			break;
		case 0x1038:
			if(catID==3) revID = 'X';
			break;
		case 0x2000:
			if((catID==2) || (catID==5)) revID = 'B';
			break;
		case 0x2008:
			if(catID==2) revID = 'Y';
			if(catID==5) revID = 'Z';
			break;
	}
#else
	#error Define ReadDeviceID for your processor !
#endif
}

// -----------------------------------------------------------------------------

void CPU_Info::ReadInfo(void)
{
	ReadDeviceID();
	ReadUniqueID();
}

// -----------------------------------------------------------------------------

uint8_t  CPU_Info::GetDevID(void)        { return catID; }
char     CPU_Info::GetRevID(void)        { return revID; }

// -----------------------------------------------------------------------------

uint16_t CPU_Info::GetFlashMemSize(void)
{
	uint16_t* ptr;

#ifdef STM32L051xx
	// "Flash size register" 0x1FF8 007C (16-bit value)
	ptr = (uint16_t*)0x1FF8007C;
	return *ptr;
#else
	#error Define ReadDeviceMemory for your processor !
	return 0;
#endif
}

// -----------------------------------------------------------------------------

void CPU_Info::ReadUniqueID(void)
{
	uint32_t* ptr;

#ifdef STM32L051xx
	// Unique device ID: 0x1FF8 0050, offset 0x00, 0x04 and 0x14 (3 * 32 bytes)
	ptr = (uint32_t*)0x1FF80050;
	uniqueID[0] = *ptr;
	ptr++;		// offset 0x04;
	uniqueID[1] = *ptr;
	ptr += 4;	// offset 0x14;
	uniqueID[2] = *ptr;
	return;
#else
	#error Define ReadUniqueID for your processor !
#endif
}

// -----------------------------------------------------------------------------

uint32_t CPU_Info::Get_CPUID_Implementer(void)
{
	return (SCB->CPUID & SCB_CPUID_IMPLEMENTER_Msk) >> SCB_CPUID_IMPLEMENTER_Pos;
}
uint32_t CPU_Info::Get_CPUID_Variant(void)
{
	return (SCB->CPUID & SCB_CPUID_VARIANT_Msk) >> SCB_CPUID_VARIANT_Pos;
}
uint32_t CPU_Info::Get_CPUID_Architecture(void)
{
	return (SCB->CPUID & SCB_CPUID_ARCHITECTURE_Msk) >> SCB_CPUID_ARCHITECTURE_Pos;
}
uint32_t CPU_Info::Get_CPUID_PartNo(void)
{
	return (SCB->CPUID & SCB_CPUID_PARTNO_Msk) >> SCB_CPUID_PARTNO_Pos;
}
uint32_t CPU_Info::Get_CPUID_Revision(void)
{
	return (SCB->CPUID & SCB_CPUID_REVISION_Msk) >> SCB_CPUID_REVISION_Pos;
}

// -----------------------------------------------------------------------------
}
