/**
 * created 2016.02.19 by Calin Radoni
 */

#ifndef MainBoard_H_
#define MainBoard_H_

#include "MainBoard_Base.h"

/** DMA Usage:
 * 	DMA1 Ch.1: memory-to-memory transfer (see cpu_DMA.h)
 * 	DMA1 Ch.2: SPI1 Rx
 * 	DMA1 Ch.3: SPI1 Tx
 * 	DMA1 Ch.4: unused
 * 	DMA1 Ch.5: unused
 * 	DMA1 Ch.6: unused
 * 	DMA1 Ch.7: unused
 */

/** Timers usage:
 * 	TIM2  - reserved for PWM generation on PA0 - PA3
 * 	TIM6  - free
 * 	TIM21 - used for entropy gathering
 * 	TIM22 - free, may be used for PWM generation on PB4 and PB5
 */

namespace PAx5 {

class MainBoard {
protected:
	void ConfigureClock(void);
	void ConfigureGPIO(void);
	void ConfigureEXTI(void);
	void ConfigureTIM6(void);

	void BlinkError(void);

	void EnableTIM6(void);
	void DisableTIM6(void);

	volatile bool radioIntFired;

	uint32_t boardCapabilities;

public:
	MainBoard();

	/** @brief Initialize the board
	 *
	 * Initialize the board and common peripherals.
	 *
	 * @param capabilities of this board, build from PAx5_BOARD_HW_* defines
	 */
	void InitializeBoard(uint32_t);

	void InitializeADCInput(uint8_t);	// [0:3] = the inputs that should be enabled

	void RadioIntFired(void);
	void CheckRadioInterrupt(void);
};

extern MainBoard board;

}

#endif /* MainBoard_H_ */
