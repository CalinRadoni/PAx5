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
 * 	TIM22 - free        (may be used for PWM generation on PB4 and PB5)
 */

namespace PAx5 {

// -----------------------------------------------------------------------------

class BoardDefinion{
public:
	BoardDefinion();

//	BoardDefinion& operator=(const BoardDefinion&);

	enum class BoardType : uint8_t
	{
		PAx5_BaseBoard,
		PAx5_EnvSensor
	};

	bool Use_USART;
	bool Use_I2C;
	bool Use_I2C_Slave;
	bool ExtFLASH;
	bool Radio_RFM69HW;
	bool PowerPeripherals_PA3;

	void SetByType(BoardType);
};

// -----------------------------------------------------------------------------

class MainBoard {
public:
	MainBoard();

	enum class Error : uint8_t
	{
		OK					= 0,
		HSI_Timeout			= 1,
		PLL_Timeout			= 2,
		ClockSwitch_Timeout	= 3,
		USART				= 4,
		Radio               = 5,
		NotInitialized      = 31
	};

	/**
	 * \brief Initialize the board
	 * \details Initialize the board and common peripherals.
	 * \param boardDef The definition of the current board
	 * \return Error::OK if the configuration succeeded. The Error::code for the first failed operation.
	 */
	MainBoard::Error InitializeBoard(BoardDefinion& boardDef);

	/**
	 * \brief Show #brdErr using the board's LED
	 * \details A long pulse followed by a number of short pulse according to the Error::code.
	 *          If Error::code is Error::OK the function does nothing.
	 */
	void BlinkError(void);

	void RadioIntFired(void);
	void CheckRadioInterrupt(void);

protected:
	MainBoard::Error brdErr;

	void ConfigureClock(void);
	void ConfigureGPIO(void);
	void ConfigureEXTI(void);
	void ConfigureTIM6(void);

	void EnableTIM6(void);
	void DisableTIM6(void);

	volatile bool radioIntFired;

	BoardDefinion boardCapabilities;
};

extern MainBoard board;

}

#endif /* MainBoard_H_ */
