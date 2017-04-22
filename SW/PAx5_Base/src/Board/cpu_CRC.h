/**
 * created 2016.11.14 by Calin Radoni
 */

#ifndef CPU_CRC_H_
#define CPU_CRC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_CRC {
public:
	CPU_CRC();
	~CPU_CRC();

	void Enable(void);
	void Disable(void);

	void AddData32(uint32_t);
	void AddData8(uint8_t);
	uint32_t GetData(void);
};

extern CPU_CRC sCRC;

} /* namespace */

#endif
