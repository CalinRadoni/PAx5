/**
 * created 2016.05.05 by Calin Radoni
 */

#include "ext_WS2812.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

EXT_WS2812 ws2812;

// -----------------------------------------------------------------------------

EXT_WS2812::EXT_WS2812() {
	wsPin = wsPIN_PB5;
	inverted = 1;

	ledCnt = 1;
	ledIdx = 0xFF;
}

EXT_WS2812::~EXT_WS2812() {
}

// -----------------------------------------------------------------------------

void EXT_WS2812::RemapPin(void)
{
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;

	// PA7 = Output, value LOW
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE7)) | GPIO_MODER_MODE7_0;
	GPIOA->BSRR  =  GPIO_BSRR_BR_7;

	// Alternate function (AFR[0] is AFRL (pin0-7) and AFR[1] is AFRH (pin8-15))
	switch(wsPin){
		case wsPIN_PA12:
			// PA12 configured as alternate function. SPI1 is AF0 (0000) for PA12
			GPIOA->MODER  = (GPIOA->MODER & ~(GPIO_MODER_MODE12)) | GPIO_MODER_MODE12_1;
			GPIOA->AFR[1] = (GPIOA->AFR[1] & 0xFFF0FFFF);
			break;
		case wsPIN_PB5:
			// PB5 configured as alternate function. SPI1 is AF0 (0000) for PB5
			GPIOB->MODER  = (GPIOB->MODER & ~(GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_1;
			GPIOB->AFR[0] = (GPIOB->AFR[0] & 0xFF0FFFFF);
			break;
	}
}
void EXT_WS2812::RestorePin(void)
{
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;

	switch(wsPin){
		case wsPIN_PA12:
			// PA12 = Output, value LOW/HIGH
			GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE12)) | GPIO_MODER_MODE12_0;
			if(inverted == 0) GPIOA->BSRR  =  GPIO_BSRR_BR_12;
			else              GPIOA->BSRR  =  GPIO_BSRR_BS_12;
			break;
		case wsPIN_PB5:
			// PB5 = Output, value LOW/HIGH
			GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;
			if(inverted == 0) GPIOB->BSRR  =  GPIO_BSRR_BR_5;
			else              GPIOB->BSRR  =  GPIO_BSRR_BS_5;
			break;
	}

	// Alternate function (AFR[0] is AFRL (pin0-7) and AFR[1] is AFRH (pin8-15))
	// PA7 configured as alternate function. SPI1 is AF0 (0000) for PA7
	GPIOA->MODER  = (GPIOA->MODER & ~(GPIO_MODER_MODE7)) | GPIO_MODER_MODE7_1;
	GPIOA->AFR[0] = (GPIOA->AFR[0] & 0x0FFFFFFF);
}

void EXT_WS2812::Initialize(uint8_t pinIn)
{
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;

	wsPin = pinIn;
	switch(wsPin){
		case wsPIN_PA12:
			// PA12 = Output push-pull, Very high speed, value LOW/HIGH
			GPIOA->MODER   = (GPIOA->MODER & ~(GPIO_MODER_MODE12)) | GPIO_MODER_MODE12_0;
			GPIOA->OTYPER  =  GPIOA->OTYPER & ~(GPIO_OTYPER_OT_12);
			GPIOA->OSPEEDR =  GPIOA->OSPEEDR | GPIO_OSPEEDER_OSPEED12;
			if(inverted == 0) GPIOA->BSRR = GPIO_BSRR_BR_12;
			else              GPIOA->BSRR = GPIO_BSRR_BS_12;
			break;
		case wsPIN_PB5:
			// PB5 = Output push-pull, Very high speed, value LOW/HIGH
			GPIOB->MODER   = (GPIOB->MODER & ~(GPIO_MODER_MODE5)) | GPIO_MODER_MODE5_0;
			GPIOB->OTYPER  =  GPIOB->OTYPER & ~(GPIO_OTYPER_OT_5);
			GPIOB->OSPEEDR =  GPIOB->OSPEEDR | GPIO_OSPEEDER_OSPEED5;
			if(inverted == 0) GPIOB->BSRR = GPIO_BSRR_BR_5;
			else              GPIOB->BSRR = GPIO_BSRR_BS_5;
			break;
	}
}

// -----------------------------------------------------------------------------

void EXT_WS2812::InitData(void)
{
	ledIdx = 0;
	if(ledCnt > wsMAX_LED_CNT)
		ledCnt = wsMAX_LED_CNT;
	dataLen = 0;
}

void EXT_WS2812::AddData(uint8_t rrr, uint8_t ggg, uint8_t bbb)
{
	uint8_t vvv, ooo;

	// G,R,B

	if(ledIdx >= ledCnt) return;
	ledIdx++;

	uint8_t byteCnt, bitSeq;
	for(byteCnt = 0; byteCnt < 3; byteCnt++){
		if(byteCnt == 0)     vvv = ggg;
		else
			if(byteCnt == 1) vvv = rrr;
			else             vvv = bbb;
		for(bitSeq = 0; bitSeq < 4; bitSeq++){
			switch(vvv & 0xC0){
				case 0x00: ooo = (inverted == 0 ? 0x88 : 0x77); break;
				case 0x40: ooo = (inverted == 0 ? 0x8E : 0x71); break;
				case 0x80: ooo = (inverted == 0 ? 0xE8 : 0x17); break;
				case 0xC0: ooo = (inverted == 0 ? 0xEE : 0x11); break;
				default: ooo = 0; break;
			}
			if(dataLen < wsBUFFER_SIZE)
				dataBuf[dataLen++] = ooo;
			vvv = vvv << 2;
		}
	}
}

void EXT_WS2812::AddData(uint32_t lll)
{
	uint8_t vvv, ooo;

	// G,R,B

	if(ledIdx >= ledCnt) return;
	ledIdx++;

	uint8_t byteCnt, bitSeq;
	for(byteCnt = 0; byteCnt < 3; byteCnt++){
		if(byteCnt == 0)     vvv = ((lll >> 8)  & 0xFF);
		else
			if(byteCnt == 1) vvv = ((lll >> 16) & 0xFF);
			else             vvv = ( lll        & 0xFF);
		for(bitSeq = 0; bitSeq < 4; bitSeq++){
			switch(vvv & 0xC0){
				case 0x00: ooo = (inverted == 0 ? 0x88 : 0x77); break;
				case 0x40: ooo = (inverted == 0 ? 0x8E : 0x71); break;
				case 0x80: ooo = (inverted == 0 ? 0xE8 : 0x17); break;
				case 0xC0: ooo = (inverted == 0 ? 0xEE : 0x11); break;
				default: ooo = 0; break;
			}
			if(dataLen < wsBUFFER_SIZE)
				dataBuf[dataLen++] = ooo;
			vvv = vvv << 2;
		}
	}
}

// -----------------------------------------------------------------------------

void EXT_WS2812::SendSPIBuffer(void)
{
	sSPI.defaultSPISettings = false;

	ocd = sSPI.clockDivider;
	sSPI.clockDivider = 2;
	sSPI.Configure();
	RemapPin();

	if(dataLen < wsBUFFER_SIZE){
		if(inverted == 0) dataBuf[dataLen++] = 0x00;
		else              dataBuf[dataLen++] = 0xFF;
	}
	sSPI.SendBufferAndWait(dataBuf, dataLen, SPI_SLAVE_NONE);

	RestorePin();
	sSPI.clockDivider = ocd;
	sSPI.Configure();

	sSPI.defaultSPISettings = true;
}

// -----------------------------------------------------------------------------
}
