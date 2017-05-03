/**
 * created 2016.02.20 by Calin Radoni
 */

#include "MainBoard_Base.h"

namespace PAx5 {

// -----------------------------------------------------------------------------

volatile uint32_t sysTickCnt = 0;

// -----------------------------------------------------------------------------

void Delay(uint32_t msIn){
	uint32_t timeStart = sysTickCnt;

	while((sysTickCnt - timeStart) < msIn)
	{
		__NOP();
	}
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------
// Cortex-M0 Plus Processor Exceptions Handlers
// -----------------------------------------------------------------------------

extern "C" void NMI_Handler(void)
{
}

extern "C" void HardFault_Handler(void)
{
#if 0
	// enable this to debug hard fault exception
	__ASM volatile("BKPT #01");
#endif

	NVIC_SystemReset();
}

extern "C" void SVC_Handler(void)
{
}

extern "C" void PendSV_Handler(void)
{
}

extern "C" void SysTick_Handler(void)
{
	PAx5::sysTickCnt++;
}

// -----------------------------------------------------------------------------
