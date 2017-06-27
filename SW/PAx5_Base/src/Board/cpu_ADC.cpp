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

#include "cpu_ADC.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_ADC sADC;

// -----------------------------------------------------------------------------

// calibration values from the reference manual
#define Vrefint_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF80078))
#define TEMP130_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF8007E))
#define TEMP30_CAL_ADDR  ((uint16_t*) ((uint32_t) 0x1FF8007A))

// -----------------------------------------------------------------------------

CPU_ADC::CPU_ADC()
{
	oversamplingRatio = 0;
}

// -----------------------------------------------------------------------------

bool CPU_ADC::Enable(uint8_t clockPrescaler, uint32_t inputClockFrequency, uint8_t samplingTime)
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	// stop ADC if started
	if((ADC1->CR & ADC_CR_ADSTART) != 0) {
		ADC1->CR |= ADC_CR_ADSTP;
		while((ADC1->CR & ADC_CR_ADSTP) != 0) { __NOP(); } // wait
	}
	// disable ADC
	if(ADC1->CR & ADC_CR_ADEN){
		ADC1->CR |= ADC_CR_ADDIS;
		while((ADC1->CR & ADC_CR_ADEN) != 0) { __NOP(); } // wait
	}

	NVIC_DisableIRQ(ADC1_COMP_IRQn);

	ADC1->CFGR1 = ADC_CFGR1_AUTOFF | ADC_CFGR1_WAIT | ADC_CFGR1_OVRMOD; // single conversion mode, overwrite old data even if not readed

	/*
	 * ADC->CFGR2 bits CKMODE[1:0] selects the input clock
	 * 00: input clock is HSI16 -> ADC clock cycle = 62.5 ns (if HSI16 is not divided by 4)
	 * 01: PCLK2 / 2
	 * 10: PCLK2 / 4
	 * 11: PCLK2 - warning: for this mode PCLK2 must have 50% duty cycle, see manual
	 */

	uint32_t val;
	uint32_t clockFrequency;

	// select ADCCLK, oversampler disabled
	ADC1->CFGR2 = 0;

	// compute resulting frequency
	clockFrequency = inputClockFrequency;
	switch(clockPrescaler){
		case  0: val =   1; break;
		case  1: val =   2; break;
		case  2: val =   4; break;
		case  3: val =   6; break;
		case  4: val =   8; break;
		case  5: val =  10; break;
		case  6: val =  12; break;
		case  7: val =  16; break;
		case  8: val =  32; break;
		case  9: val =  64; break;
		case 10: val = 128; break;
		case 11: val = 256; break;
		default: val =   0; break;
	}
	if(val == 0)
		return false;
	clockFrequency /= val;

	// and set the prescaler and LFMEM
	val = clockPrescaler;
	val = val << ADC_CCR_PRESC_Pos;
	if(clockFrequency < 3500000U)
		val |=   ADC_CCR_LFMEN ;
	ADC1_COMMON->CCR = val;

	// sampling time
	if(samplingTime > 7)
		return false;
	ADC1->SMPR  = samplingTime;

	// select channel 0
	ADC1->CHSELR = ADC_CHSELR_CHSEL0;

	// clear flags ...
	ADC1->ISR = ADC_ISR_EOCAL | ADC_ISR_AWD | ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC | ADC_ISR_EOSMP | ADC_ISR_ADRDY;
	// ... no interrupts
	ADC1->IER = 0;

	ADC1->CR |= ADC_CR_ADCAL;                            // calibrate ADC :)) ...
	while((ADC1->ISR & ADC_ISR_EOCAL) == 0) { __NOP(); } // ... wait ...
	ADC1->ISR |= ADC_ISR_EOCAL;                          // ... and clear the flag

	// enable ADC
	ADC1->CR |= ADC_CR_ADEN;
	if((ADC1->CFGR1 &  ADC_CFGR1_AUTOFF) == 0) {
		while((ADC1->ISR & ADC_ISR_ADRDY) == 0) { __NOP(); } // wait
	}

	return true;
}

void CPU_ADC::ReadChannel(uint8_t channelIn)
{
	uint8_t  channel;
	uint32_t ch32;
	uint32_t ovsVal, val;

	channel = channelIn;
	if(channel > 18) channel = 18;
	ch32 = 1 << channel;

	// stop ADC if started
	if((ADC1->CR & ADC_CR_ADSTART) != 0) {
		ADC1->CR |= ADC_CR_ADSTP;
		while((ADC1->CR & ADC_CR_ADSTP) != 0) { __NOP(); } // wait
	}

	if(channel == 18){
		// enable temperature sensor
		ADC->CCR |= ADC_CCR_TSEN;
	}
	if(channel == 17){
		// enable Vrefint
		ADC->CCR |= ADC_CCR_VREFEN;
	}

	ADC1->CHSELR = ch32; // select channel

	if(oversamplingRatio <= 7){
		// set oversampling shift
		ovsVal  = oversamplingRatio + 1;
		ovsVal  = ovsVal << ADC_CFGR2_OVSS_Pos;
		// set oversampling ratio
		ovsVal |= oversamplingRatio << ADC_CFGR2_OVSR_Pos;
		// enable oversampler
		ovsVal |= ADC_CFGR2_OVSE;
	}
	else ovsVal = 0;

	val = ADC1->CFGR2 & ~(ADC_CFGR2_OVSS | ADC_CFGR2_OVSR | ADC_CFGR2_OVSE);

	// for the first read ... no oversampling
	ADC1->CFGR2 = val;

	// ignore first reading as per Reference Manual/Datasheet errata
	ADC1->ISR |= (ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC); // clear flags
	ADC1->CR |= ADC_CR_ADSTART; // start
	while((ADC1->ISR & ADC_ISR_EOC) == 0){ __NOP(); } // wait for conversion to end
	data = ADC1->DR;

	// for the second read ... add oversampling
	val |= ovsVal;
	ADC1->CFGR2 = val;

	// this is the good reading
	ADC1->ISR |= (ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC); // clear flags
	ADC1->CR |= ADC_CR_ADSTART; // start
	while((ADC1->ISR & ADC_ISR_EOC) == 0){ __NOP(); } // wait for conversion to end
	data = ADC1->DR;

	if(channel == 18){
		// disable temperature sensor
		ADC->CCR &= ~(ADC_CCR_TSEN);
	}
	if(channel == 17){
		// disable Vrefint
		ADC->CCR &= ~(ADC_CCR_VREFEN);
	}
}

void CPU_ADC::Disable(void)
{
	NVIC_DisableIRQ(ADC1_COMP_IRQn);

	// stop ADC if started
	if((ADC1->CR & ADC_CR_ADSTART) != 0) { ADC1->CR |= ADC_CR_ADSTP; }
	while((ADC1->CR & ADC_CR_ADSTP) != 0) { __NOP(); } // wait
	// disable ADC
	ADC1->CR |= ADC_CR_ADDIS;
	while((ADC1->CR & ADC_CR_ADEN) != 0) { __NOP(); } // wait
	// disable ADC interrupts
	ADC1->IER = 0;
	// disabled ADC voltage regulator
	ADC1->CR = ADC1->CR & ~(ADC_CR_ADVREGEN);
	// disable temperature sensor and Vrefint
	ADC->CCR = ADC->CCR & ~(ADC_CCR_TSEN | ADC_CCR_VREFEN);

	RCC->APB2ENR = RCC->APB2ENR & ~(RCC_APB2ENR_ADC1EN);
}

// -----------------------------------------------------------------------------

void CPU_ADC::ReadVDDA(void)
{
	ReadChannel(17);

	VddA  = (uint32_t)3000;
	VddA *= (uint32_t)(*Vrefint_CAL_ADDR);
	VddA /= (uint32_t)data;
	VddA += 5;
	VddA /= 10;
	data  = (uint16_t)VddA;
}

void CPU_ADC::ConvertDataToVoltage(void)
{
	uint32_t val;

	val  = VddA;
	val *= (uint32_t)data;
	val /= (uint32_t)4095;
	data = (uint16_t)val;
}

int8_t CPU_ADC::ConvertDataToTemperature(void)
{
	int32_t temp;

	temp  = (int32_t)data;
	temp *= (int32_t)VddA;
	temp /= 30;
	temp -= 10 * (int32_t)(*TEMP30_CAL_ADDR);
	temp *= (int32_t)(130 - 30);
	temp /= (int32_t)(*TEMP130_CAL_ADDR - *TEMP30_CAL_ADDR);
	if(temp > 0) temp += 5;
	else         temp -= 5;
	temp /= 10;
	temp += 30;

	return (int8_t)temp;
}

void CPU_ADC::GetC1(void) { data = *Vrefint_CAL_ADDR; }
void CPU_ADC::GetC2(void) { data = *TEMP30_CAL_ADDR; }
void CPU_ADC::GetC3(void) { data = *TEMP130_CAL_ADDR; }

// -----------------------------------------------------------------------------
}
