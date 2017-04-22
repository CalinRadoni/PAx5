/**
 * created 2016.05.05 by Calin Radoni
 */

#ifndef ext_WS2812_H_
#define ext_WS2812_H_

#include "cpu_SPI1.h"

namespace PAx5 {

#define wsPIN_PA7  0
#define wsPIN_PA12 1
#define wsPIN_PB5  2

#define wsMAX_LED_CNT 10
// 3 color bytes per LED
// 2 color bits per output byte
// 1 byte for MOSI idle state
#define wsBUFFER_SIZE (1 + (12 * wsMAX_LED_CNT))

class EXT_WS2812 {
protected:
	uint8_t wsPin;
	uint8_t ocd;
	uint8_t ledIdx;
	void ConfigurePA12(void);
	void ConfigurePB5(void);
	void RemapPin(void);
	void RestorePin(void);

	uint8_t  dataBuf[wsBUFFER_SIZE];
	uint16_t dataLen;

public:
	EXT_WS2812();
	virtual ~EXT_WS2812();

	void Initialize(uint8_t); // the pin used

	uint8_t inverted; // if is an inverting buffer between STM32 and the first WS2812

	uint8_t ledCnt; // number of WS2812 LEDs, adjusted in 'InitData' !

	void InitData(void);
	void AddData(uint8_t, uint8_t, uint8_t);
	void AddData(uint32_t);

	// !!! Call board.CheckRadioInterrupt() after this function !!!
	void SendSPIBuffer(void);
};

extern EXT_WS2812 ws2812;

} /* namespace */
#endif /* ext_WS2812_H_ */
