/**
 * created 2016.06.16 by Calin Radoni
 */

#ifndef cpu_INFO_H_
#define cpu_INFO_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_Info {
protected:
	uint8_t catID;				///< Device identifier aka MCU's category (1, 2, 3, ...)
	char revID;					///< Revision identifier ('A', 'B', ...)

	void ReadDeviceID(void);
	void ReadDeviceMemory(void);

public:
	CPU_Info();
	virtual ~CPU_Info();

	/** Reads MCU specific info.
	 *
	 * This function fills Category, Revision ID, and Unique ID fields.
	 */
	void ReadInfo(void);

	uint8_t  GetDevID(void);
	char     GetRevID(void);

	uint16_t GetFlashMemSize(void);	///< Returns Flash memory size in Kbytes


	/** MCU Unique ID
	 *
	 * This function is called from ReadInfo().
	 *
	 * From STM32L051 Reference Manual - RM0377 (DocID 025942 Rev.5), 28.Device electronic signature:
	 * STM32L0x1 format for UID is:
	 * 	uid[0](31:24) = WAF_NUM[7:0]
	 * 	uid[0](23:0)  = LOT_NUM[55:32]
	 * 	uid[1](31:0)  = LOT_NUM[31:0]
	 * 	uid[2](31:0)  = unique ID bits
	 */
	uint32_t uniqueID[3];
	void ReadUniqueID(void);

	// These functions get informations from SCB's CPUID register
	uint32_t Get_CPUID_Implementer(void);
	uint32_t Get_CPUID_Variant(void);
	uint32_t Get_CPUID_Architecture(void);
	uint32_t Get_CPUID_PartNo(void);
	uint32_t Get_CPUID_Revision(void);
};

extern CPU_Info sCPU;

}
#endif /* cpu_INFO_H_ */
