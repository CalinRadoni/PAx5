/**
 * created 2016.03.12 by Calin Radoni
 */

#include "cpu_Entropy.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_Entropy entropy;

// -----------------------------------------------------------------------------

CPU_Entropy::CPU_Entropy()
{
	enabled = false;
	collectState = 0;
	buffHead = 0;
	buffLen = 0;
}

CPU_Entropy::~CPU_Entropy() {
	Disable();
}

// -----------------------------------------------------------------------------

uint8_t CPU_Entropy::BytesAvailable(void)
{
	return buffLen;
}

uint8_t CPU_Entropy::Get8bits(void)
{
	uint8_t val;

	if(buffLen == 0){
		CollectStart();
		while(buffLen == 0) __NOP(); // wait
	}

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	val = buffer[buffHead];
	buffLen--;
	buffHead = (buffHead + 1) % EntropyBufferLen;

	if(pMask == 0) __enable_irq();

	return val;
}

uint32_t CPU_Entropy::Get32bits(void)
{
	uint32_t res;
	uint8_t val;

	if(buffLen < 4){
		CollectStart();
		while(buffLen < 4) __NOP(); // wait
	}

	uint32_t pMask = __get_PRIMASK();
	__disable_irq();

	val = buffer[buffHead]; buffLen--; buffHead = (buffHead + 1) % EntropyBufferLen;
	res  = val; res = res << 8;
	val = buffer[buffHead]; buffLen--; buffHead = (buffHead + 1) % EntropyBufferLen;
	res |= val; res = res << 8;
	val = buffer[buffHead]; buffLen--; buffHead = (buffHead + 1) % EntropyBufferLen;
	res |= val; res = res << 8;
	val = buffer[buffHead]; buffLen--; buffHead = (buffHead + 1) % EntropyBufferLen;
	res |= val;

	if(pMask == 0) __enable_irq();

	return res;
}

// -----------------------------------------------------------------------------

void CPU_Entropy::Enable(void)
{
	NVIC_DisableIRQ(TIM21_IRQn);

	// Configure LSI
	RCC->CIER = RCC->CIER & ~(RCC_CIER_LSIRDYIE); // no interrupt when LSI is stable

	if((RCC->CSR & RCC_CSR_LSION) != 0){
		statusLSI = true;
	}
	else{
		statusLSI = false;
		RCC->CSR |= RCC_CSR_LSION; // Start LSI
	}

	// Enable the clock for TM21
	RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;

	// Configure TIM21
	TIM21->CR1 = TIM_CR1_ARPE | TIM_CR1_URS | TIM_CR1_UDIS;
	TIM21->CR2 = 0;
	TIM21->SMCR = 0; // master mode
	TIM21->DIER = 0;
	TIM21->EGR = 0;
	TIM21->CCER = 0; // to enable writing in CCMR1
	TIM21->CCMR1 = TIM_CCMR1_CC1S_0; // CC1 channel is configured as input, IC1 is mapped on TI1
	TIM21->PSC = 0;
	TIM21->ARR = 0xFFFF;
	TIM21->OR = TIM21_OR_TI1_RMP_2 | TIM21_OR_TI1_RMP_0; // TIM21 TI1 input connected to LSI clock
	TIM21->SR = 0; // clear flags
	TIM21->CCER = TIM_CCER_CC1E; // enable capture
	TIM21->DIER = TIM_DIER_CC1IE;

	NVIC_SetPriority(TIM21_IRQn, NVIC_PRIORITY_TIM21);

	enabled = true;
}

void CPU_Entropy::CollectStart(void)
{
	if(!enabled) Enable();

	collectState = 0;

	TIM21->SR = 0;                 // clear flags
	NVIC_EnableIRQ(TIM21_IRQn);    // enable interrupt
	TIM21->CR1 |= TIM_CR1_CEN;     // start timer
}

bool CPU_Entropy::EntropyCollected(void)
{
	return (collectState == 0xFF) ? true : false;
}

void CPU_Entropy::CollectStop(void)
{
	NVIC_DisableIRQ(TIM21_IRQn);                // disable interrupt
	TIM21->CR1 = TIM21->CR1 & ~(TIM_CR1_CEN);   // stop TIM21
}

void CPU_Entropy::Disable(void)
{
	NVIC_DisableIRQ(TIM21_IRQn);                          // disable interrupt
	TIM21->CR1 = TIM21->CR1 & ~(TIM_CR1_CEN);             // stop TIM21
	RCC->APB2ENR = RCC->APB2ENR & ~(RCC_APB2ENR_TIM21EN); // stop the clock for TIM21
	if(!statusLSI){
		RCC->CSR = RCC->CSR & ~(RCC_CSR_LSION);           // stop LSI
	}

	enabled = false;
}

// -----------------------------------------------------------------------------

void CPU_Entropy::Collect(void)
{
	Enable();
		CollectStart();
		while(!EntropyCollected()) { __NOP(); } // wait
	Disable();
}

// -----------------------------------------------------------------------------

void CPU_Entropy::HandleTimerInterrupt(void)
{
	uint16_t cntResult, cnt;
	uint8_t newState;
	uint8_t wIdx;

	if((TIM21->SR & TIM_SR_CC1IF) != 0){
		cnt = TIM21->CCR1;
		if((TIM21->SR & TIM_SR_CC1OF) != 0){ // overflow
			TIM21->SR = TIM21->SR & ~(TIM_SR_CC1OF);
		}

		switch(collectState){
		case 0:
			newState = 1;
			cntStart = cnt; ///< cntStart = TIM21 value
			break;

		case 1:
			newState = 2;
			cntStop = cnt;
			if(cntStop > cntStart){
				cntResult = cntStop - cntStart;
			}
			else{
				cntResult = 0xFFFF - cntStart;
				cntResult += cntStop;
				cntResult++;
			}
			cntStart = cntStop; ///< cntStart = TIM21 value
			cntVal = cntResult; ///< cntVal = ticks counted by TIM21 in current iteration

			buffMask = 0x01;
			whVal = 0; whCnt = 8;
			break;

		case 2:
			newState = collectState;
			cntStop = cnt;
			if(cntStop > cntStart){
				cntResult = cntStop - cntStart;
			}
			else{
				cntResult = 0xFFFF - cntStart;
				cntResult += cntStop;
				cntResult++;
			}
			cntStart = cntStop;

			/**
			 * cntStart  = TIM21 value
			 * cntVal    = ticks counted by TIM21 in previous iteration
			 * cntResult = ticks counted by TIM21 in current iteration
			 */

			if(cntResult > cntVal)
				whVal++;
			cntVal = cntResult; ///< cntVal = ticks counted by TIM21 in current iteration
			--whCnt;
			if(whCnt == 0){
				// 8 iterations done

				if(buffMask == 0x01)    entropyByte = 0;
				if((whVal & 0x01) != 0) entropyByte |= buffMask;

				whCnt = 8;
				whVal = 0;

				// now the current bit is set, and whVal and whCnt are initialized for next bit

				buffMask = buffMask << 1;
				if(buffMask == 0){
					// all bits of the current byte are set

					if(buffLen >= EntropyBufferLen){
						// overflow, overwrite oldest value
						buffLen = EntropyBufferLen - 1;
						buffHead = (buffHead + 1) % EntropyBufferLen;
					}

					// store current byte
					wIdx = (buffHead + buffLen) % EntropyBufferLen;
					buffer[wIdx] = entropyByte;

					buffLen++;
					if(buffLen == EntropyBufferLen){
						// buffer is full, stop collection
						CollectStop();
						newState = 0xFF;
					}
					else{
						// prepare for next byte
						buffMask = 0x01;
					}
				}
			}
			break;

		case 0xFF:
			// nothing to do, should not arrive here, this is the end state
			newState = collectState;
			break;

		default:
			// something is wrong, should not arrive here, go to first state
			newState = 0;
			break;
		}

		collectState = newState;
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void TIM21_IRQHandler(void)
{
	PAx5::entropy.HandleTimerInterrupt();
}

// -----------------------------------------------------------------------------
