/**
 * created 2016.03.12 by Calin Radoni
 */

#ifndef CPU_Entropy_H_
#define CPU_Entropy_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t EntropyBufferLen = 32;

/** @class CPU_Entropy
 *
 * Collects some random values based on clock jitter.
 */
class CPU_Entropy {
public:
	CPU_Entropy();
	virtual ~CPU_Entropy();

	volatile uint8_t buffer[EntropyBufferLen];

	uint8_t BytesAvailable(void);
	uint8_t Get8bits(void);
	uint32_t Get32bits(void);

	/** Enable entropy collection by timer
	 *
	 * Enable and configure LSI, TIM21 and TIM21_IRQn.
	 * To start collecting entropy call the Collect function.
	 */
	void Enable(void);

	/** Start entropy collection
	 *
	 * This function starts the collection of entropy bytes using TIM21.
	 * The function returns immediately.
	 * To check if entropy buffer is full call the EntropyCollected function.
	 *
	 * For a single entropy buffer collection there is the function CollectFromTimer.
	 *
	 * @code{.cpp}
	 * entropy.Enable();
	 * ...
	 * entropy.CollectStart();
	 * while(!EntropyCollected()) { __NOP(); }
	 * ...
	 * entropy.CollectStart();
	 * while(!EntropyCollected()) { __NOP(); }
	 * ...
	 * entropy.Disable();
	 * @endcode
	 *
	 * @param parameterName parameterDescription
	 * @return returnDescription
	 */
	void CollectStart(void);

	/** Stop entropy collection by timer
	 *
	 * Normally, there is no need for user to call this function.
	 * This function is automatically called when the entropy buffer is full.
	 */
	void CollectStop(void); // called automatically when buffer is full

	/** Disable entropy collection by timer */
	void Disable(void);

	/** Check if entropy collection is over */
	bool EntropyCollected(void);

	/** Collect a full buffer from timer
	 *
	 * This is a shortcut function for Enable(), CollectStart(), wait until EntropyCollected()==true and Disable() sequence.
	 */
	void Collect(void);

	void HandleTimerInterrupt(void);

protected:
	volatile bool enabled;
	volatile bool statusLSI;

	volatile uint8_t  collectState;
	volatile uint16_t cntStart, cntStop, cntVal;
	volatile uint8_t  buffMask;
	volatile uint8_t  whVal, whCnt;
	volatile uint8_t  entropyByte;

	volatile uint8_t buffHead, buffLen;
};

extern CPU_Entropy entropy;

} /* namespace */
#endif
