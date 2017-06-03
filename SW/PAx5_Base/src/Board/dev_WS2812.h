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

	// !!! Call board.CheckRadioInterrupt() after this function !!!
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
