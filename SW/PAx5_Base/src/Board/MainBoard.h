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
		PAx5_EnvSensor,
		PAx5_CPU
	};

	bool Use_USART;
	bool Use_I2C;
	bool Use_I2C_Slave;
	bool ExtFLASH;
	bool Radio_RFM69HW;
	bool PowerPeripherals_PA3;

	uint32_t PortA, PortB, PortC;

	void SetByType(BoardType);

	bool Use_SPI(void);

protected:
};

// -----------------------------------------------------------------------------

class MainBoard {
public:
	MainBoard();

	enum class Error : uint8_t
	{
		OK                  = 0,
		Clk_HSI_Timeout     = 1,
		Clk_PLL_Timeout     = 2,
		Clk_ClockSw_Timeout	= 3,
		Clk_UnknownError    = 4,
		USART				= 5,
		Radio               = 6,
		NotInitialized      = 31
	};

	/**
	 * \brief Initialize the board
	 *
	 * \details
	 * This function:
	 * - initialize the board
	 * - powers the peripherals
	 * - initialize external FLASH memory
	 * - initialize the radio module
	 *
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

	void PeripheralsOn(void);
	void PeripheralsOff(void);

	void Sleep(void);
	void Standby(void);

	void RadioIntFired(void);
	void CheckRadioInterrupt(void);

protected:
	MainBoard::Error brdErr;

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
