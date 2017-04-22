/**
 * created 2016.03.14 by Calin Radoni
 */

#include "cpu_EntropyADC.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_EntropyADC entropyADC;

// -----------------------------------------------------------------------------

CPU_EntropyADC::CPU_EntropyADC()
{
	collected = false;
}

CPU_EntropyADC::~CPU_EntropyADC()
{
	Disable();
}

// -----------------------------------------------------------------------------

void CPU_EntropyADC::Enable(void)
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

	ADC1->CFGR1  = ADC_CFGR1_CONT;     // continuous mode

	ADC1->CFGR2  = 0;                  // ADCCLK (Asynchronous clock mode), Oversampler disabled
	ADC1->SMPR   = 0;                  // 1.5 ADC clock cycles (at 4 MHz this is 0,375 uS - need NOT a good result !!! - 2,2 uS recommended)
	//ADC1->CFGR2 = ADC_CFGR2_CKMODE_1; // 8 MHZ
	//ADC1->SMPR = ADC_SMPR_SMP_0;      // 3.5 ADC Clock cycles

	ADC1->CHSELR = ADC_CHSELR_CHSEL18; // select channel 18
	ADC->CCR     = ADC_CCR_TSEN;       // temperature sensor enable

	// clear flags ...
	ADC1->ISR = ADC_ISR_EOCAL | ADC_ISR_AWD | ADC_ISR_OVR | ADC_ISR_EOSEQ | ADC_ISR_EOC | ADC_ISR_EOSMP | ADC_ISR_ADRDY;
	// ... and enable "End of conversion" interrupt
	ADC1->IER = ADC_IER_EOCIE;

//	ADC1->CR |= ADC_CR_ADCAL;                            // calibrate ADC :)) ...
//	while((ADC1->ISR & ADC_ISR_EOCAL) == 0) { __NOP(); } // ... wait ...
//	ADC1->ISR |= ADC_ISR_EOCAL;                          // ... and clear the flag

	// enable ADC
	ADC1->CR |= ADC_CR_ADEN;
	if((ADC1->CFGR1 &  ADC_CFGR1_AUTOFF) == 0) {
		while((ADC1->ISR & ADC_ISR_ADRDY) == 0) { __NOP(); } // wait
	}

	NVIC_DisableIRQ(ADC1_COMP_IRQn);
	NVIC_SetPriority(ADC1_COMP_IRQn, NVIC_PRIORITY_ADC);
}

void CPU_EntropyADC::CollectStart(void)
{
	collected = false;
	buffIdx = 0; buffMask = 0x01;
	whVal = 0; whCnt = 8;

	NVIC_EnableIRQ(ADC1_COMP_IRQn);
	ADC1->CR |= ADC_CR_ADSTART;
}

void CPU_EntropyADC::CollectStop(void)
{
	NVIC_DisableIRQ(ADC1_COMP_IRQn);

	// stop ADC if started
	if((ADC1->CR & ADC_CR_ADSTART) != 0) { ADC1->CR |= ADC_CR_ADSTP; }
	while((ADC1->CR & ADC_CR_ADSTP) != 0) { __NOP(); } // wait
}

void CPU_EntropyADC::Disable(void)
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
	// disable temperature sensor
	ADC->CCR = ADC->CCR & ~(ADC_CCR_TSEN);

	RCC->APB2ENR = RCC->APB2ENR & ~(RCC_APB2ENR_ADC1EN);
}

bool CPU_EntropyADC::EntropyCollected(void)
{
	return collected;
}

// -----------------------------------------------------------------------------

void CPU_EntropyADC::Collect(void)
{
	Enable();
		CollectStart();
		while(!EntropyCollected()) { __NOP(); } // wait
	Disable();
}

// -----------------------------------------------------------------------------

void CPU_EntropyADC::HandleADCInterrupt(void)
{
	uint16_t val;

	if((ADC1->ISR & ADC_ISR_OVR) != 0) {
		// overflow, ignore and clear flag
		ADC1->ISR |= ADC_ISR_OVR;
	}

	if((ADC1->ISR & ADC_ISR_EOC) != 0) {
		val = ADC1->DR; // read data, flag is cleared by reading data

		if((val & 0x0001) != 0)
			whVal++;
		--whCnt;
		if(whCnt == 0){
			whCnt = 8;

			if(buffMask == 0x01)    entropyByte = 0;
			if((whVal & 0x01) != 0) entropyByte |= buffMask;
			whVal = 0;

			buffMask = buffMask << 1;
			if(buffMask == 0){
				buffer[buffIdx] = entropyByte;
				buffMask = 0x01;
				buffIdx++;
				if(buffIdx == ENTROPY_BUFFER_LEN_ADC){
					CollectStop();
					collected = true;
				}
			}
		}
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void ADC1_COMP_IRQHandler(void)
{
	PAx5::entropyADC.HandleADCInterrupt();
}

// -----------------------------------------------------------------------------
