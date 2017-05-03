/**
 * created 2016.05.05 by Calin Radoni
 */

#ifndef ext_WS2812_H_
#define ext_WS2812_H_

#include "cpu_SPI1.h"

namespace PAx5 {

const uint16_t wsMAX_LED_CNT = 10;
// 3 color bytes per LED -> 24 color bits
// 2 color bits per output byte -> 12 bytes / LED
// 1 byte for MOSI idle state
constexpr uint16_t wsBUFFER_SIZE = (1 + (12 * wsMAX_LED_CNT));

class EXT_WS2812 {
public:
	EXT_WS2812();
	virtual ~EXT_WS2812();

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

extern EXT_WS2812 ws2812;

} /* namespace */
#endif /* ext_WS2812_H_ */
