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

#ifndef cpu_INFO_H_
#define cpu_INFO_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_Info {
public:
	CPU_Info();
	virtual ~CPU_Info();

	/** Reads MCU specific info.
	 *
	 * This function fills #uniqueID and the protected #catID and #revID fields.
	 */
	void ReadInfo(void);

	uint8_t  GetDevID(void);
	char     GetRevID(void);

	uint16_t GetFlashMemSize(void);	///< Returns Flash memory size in KBytes


	/** MCU Unique ID
	 *
	 * This function is called from ReadInfo().
	 *
	 * From STM32L051 Reference Manual - RM0377 (DocID 025942 Rev.5), 28.Device electronic signature:
	 * STM32L0x1 format for UID is:
	 * 	uniqueID[0](31:24) = WAF_NUM[7:0]
	 * 	uniqueID[0](23:0)  = LOT_NUM[55:32]
	 * 	uniqueID[1](31:0)  = LOT_NUM[31:0]
	 * 	uniqueID[2](31:0)  = unique ID bits
	 */
	uint32_t uniqueID[3];
	void ReadUniqueID(void);

	// These functions get informations from SCB's CPUID register
	uint32_t Get_CPUID_Implementer(void);
	uint32_t Get_CPUID_Variant(void);
	uint32_t Get_CPUID_Architecture(void);
	uint32_t Get_CPUID_PartNo(void);
	uint32_t Get_CPUID_Revision(void);

protected:
	uint8_t catID;				///< Device identifier a.k.a. MCU's category (1, 2, 3, ...)
	char revID;					///< Revision identifier ('A', 'B', ...)

	void ReadDeviceID(void);
	void ReadDeviceMemory(void);
};

extern CPU_Info sCPU;

}
#endif /* cpu_INFO_H_ */
