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

#ifndef dev_WS2812_H_
#define dev_WS2812_H_

#include "cpu_SPI1.h"

namespace PAx5 {

const uint16_t wsMAX_LED_CNT = 10;
// 3 color bytes per LED -> 24 color bits
// 2 color bits per output byte -> 12 bytes / LED
// 1 byte for MOSI idle state
constexpr uint16_t wsBUFFER_SIZE = (1 + (12 * wsMAX_LED_CNT));

class DEV_WS2812 {
public:
	DEV_WS2812();
	virtual ~DEV_WS2812();

	enum class WSPinName : uint8_t {
		// PA7,  // this is used for Radio module and External FLASH
		PA12, // this is free to be used for WS2812 LEDs
		PB5   // this is free to be used for WS2812 LEDs
	};

	void Initialize(WSPinName); // the pin to use

	uint8_t inverted; // if is an inverting buffer between STM32 and the first WS2812

	uint8_t ledCnt; // number of WS2812 LEDs, adjusted in 'InitData' !

	void InitData(void);
	void AddData(uint8_t, uint8_t, uint8_t);
	void AddData(uint32_t);

	/**
	 * \brief Sends the data added with #AddData functions
	 *
	 * \details This function:
	 * - save the value of SPI clock
	 * - set the SPI clock to 4 MHz
	 * - send data
	 * - set the SPI clock to saved value
	 *
	 * According to the WS2812B datasheet, the bit length is 1250 ns +/- 300 ns,
	 * which equals to 950 ns - 1550 ns bit length.
	 * The ideal value for the SPI clock is 3.2 MHz but 4 MHz is good enough and within the datasheet range.
	 *
	 * \warning Call board.CheckRadioInterrupt() after this function !
	 */
	void SendSPIBuffer(void);

protected:
	WSPinName wsPin;
	uint8_t ledIdx;

	void ConfigurePA12(void);
	void ConfigurePB5(void);
	void RemapPin(void);
	void RestorePin(void);

	uint8_t  dataBuf[wsBUFFER_SIZE];
	uint16_t dataLen;
};

extern DEV_WS2812 ws2812;

} /* namespace */
#endif /* ext_WS2812_H_ */
