/**
 * created 2016.03.14 by Calin Radoni
 */

#ifndef CPU_EntropyADC_H_
#define CPU_EntropyADC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define ENTROPY_BUFFER_LEN_ADC 8

class CPU_EntropyADC {
protected:
	void Initialize(void);

	volatile bool     collected;
	volatile uint8_t  buffIdx, buffMask;
	volatile uint8_t  whVal, whCnt;
	volatile uint8_t  entropyByte;

public:
	CPU_EntropyADC();
	virtual ~CPU_EntropyADC();

	volatile uint8_t buffer[ENTROPY_BUFFER_LEN_ADC];

	void Enable(void);
	void CollectStart(void);
	void CollectStop(void);
	void Disable(void);

	bool EntropyCollected(void);

	void Collect(void);

	void HandleADCInterrupt(void);
};

extern CPU_EntropyADC entropyADC;

} /* namespace */

#endif
