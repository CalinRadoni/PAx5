/**
 * created 2016.03.27 by Calin Radoni
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

void CPU_ADC::Enable(void)
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

	ADC1->CFGR1 = ADC_CFGR1_AUTOFF | ADC_CFGR1_WAIT | ADC_CFGR1_OVRMOD; // single conversion mode, overwrite old data even if not readed

	// total conversion time is 12.5 + SMPR ADC Clock cycles
	ADC1->CFGR2 = ADC_CFGR2_CKMODE_1; // 8 MHZ
	ADC1->SMPR  = ADC_SMPR_SMP;       // sampling time = 160.5 ADC Clock cycles (~20 us)

	ADC1->CHSELR = ADC_CHSELR_CHSEL0; // select channel 0

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
}

void CPU_ADC::ReadChannel(uint8_t channelIn)
{
	uint8_t  channel;
	uint32_t ch32;
	uint32_t ovsVal;

	channel = channelIn;
	if(channel > 18) channel = 18;
	ch32 = 1 << channel;

	// stop ADC if started
	if((ADC1->CR & ADC_CR_ADSTART) != 0) { ADC1->CR |= ADC_CR_ADSTP; }
	while((ADC1->CR & ADC_CR_ADSTP) != 0) { __NOP(); } // wait

	if(channel == 18) ADC->CCR = ADC_CCR_TSEN;   // enable temperature sensor
	if(channel == 17) ADC->CCR = ADC_CCR_VREFEN; // enable Vrefint

	ADC1->CHSELR = ch32; // select channel

	if(oversamplingRatio > 7) oversamplingRatio = 7;
	ovsVal  = oversamplingRatio + 1;  // set oversampling shift
	ovsVal  = ovsVal << 5;
	ovsVal |= oversamplingRatio << 2; // set oversampling ratio
	ovsVal |= ADC_CFGR2_OVSE;         // enable oversampler
	ovsVal |= ADC_CFGR2_CKMODE_1;     // set clock prescaller to 4

	// for the first read ...
	ADC1->CFGR2 = ADC_CFGR2_CKMODE_1;

	// ignore first reading as per Reference Manual/Datasheet errata
	ADC1->ISR |= (ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC); // clear flags
	ADC1->CR |= ADC_CR_ADSTART; // start
	while((ADC1->ISR & ADC_ISR_EOC) == 0){ __NOP(); } // wait for conversion to end
	data = ADC1->DR;

	// for the second read ...
	ADC1->CFGR2 = (oversamplingRatio==0) ? ADC_CFGR2_CKMODE_1 : ovsVal;

	// this is the good reading
	ADC1->ISR |= (ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC); // clear flags
	ADC1->CR |= ADC_CR_ADSTART; // start
	while((ADC1->ISR & ADC_ISR_EOC) == 0){ __NOP(); } // wait for conversion to end
	data = ADC1->DR;

	if(channel == 18) ADC->CCR = ADC->CCR & ~(ADC_CCR_TSEN);   // disable temperature sensor
	if(channel == 17) ADC->CCR = ADC->CCR & ~(ADC_CCR_VREFEN); // disable Vrefint
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
