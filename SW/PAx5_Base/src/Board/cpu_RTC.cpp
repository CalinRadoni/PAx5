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

#include <cpu_RTC.h>

namespace PAx5 {
// -----------------------------------------------------------------------------

FILEID(3);

const uint8_t LogCodeRTC = 1;
const uint16_t LogCodeRTC_WaitLSI = 1;
const uint16_t LogCodeRTC_WUTWF   = 2;
const uint16_t LogCodeRTC_INITF   = 3;

// -----------------------------------------------------------------------------

const uint32_t rtcTimeout = 3000U;

// see #CPU_RTC.InitializePrescaler for computing this value
const uint32_t rtcPrescaler = 0x007C0127U;

/**
 * RTC wake up auto-reload value.
 *
 * It depends on #rtcPrescaler.
 *
 * From RM0377: When the wake up timer is enabled (WUTE set to 1), the WUTF flag
 * is set every (WUT[15:0] + 1) ck_wut cycles.
 *
 * (599 + 1) cycles at 1 Hz = 600 seconds
 *
 * \warning Only the last 16 bits are used !
 */
const uint32_t rtcWut = 599U; // 10 minutes

// -----------------------------------------------------------------------------

CPU_RTC sRTC;

// -----------------------------------------------------------------------------

inline void RTC_WriteAccess_Enable(void){
	RTC->WPR = 0xCAU;
	RTC->WPR = 0x53U;
}

inline void RTC_WriteAccess_Disable(void){
	RTC->WPR = 0x0FU;
	RTC->WPR = 0xF0U;
}

// -----------------------------------------------------------------------------

CPU_RTC::CPU_RTC()
{
	rtcWakeUpFired = false;
}

CPU_RTC::~CPU_RTC(){ }

// -----------------------------------------------------------------------------

bool CPU_RTC::InitializeRTC(void)
{
	NVIC_DisableIRQ(RTC_IRQn);

	// --- clock -----------------------------------

	// enable LSI ...
	RCC->CSR |= RCC_CSR_LSION;
	// ... and wait until is ready
	uint32_t tickstart = sysTickCnt;
	while((RCC->CSR & RCC_CSR_LSIRDY) != RCC_CSR_LSIRDY){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_WaitLSI);
			return false;
		}
	}

	// enable PWR clock
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	// set DBP bit to enable write access in RTC domain
	PWR->CR |= PWR_CR_DBP;

	// configure and enable Control/status register
	uint32_t val = RCC->CSR;
	val &= ~(RCC_CSR_RTCSEL);
	val |= RCC_CSR_RTCSEL_LSI; // LSI used as RTC clock
	val |= RCC_CSR_RTCEN;      // RTC clock enable
	RCC->CSR = val;

	// --- RTC -------------------------------------

	if(!InitializePrescaler())
		return false;

	// --- interrupt -------------------------------

	// set priority ...
	NVIC_SetPriority(RTC_IRQn, NVIC_PRIORIY_RTC);
	// ... and enable interrupt
	NVIC_EnableIRQ(RTC_IRQn);

	return true;
}

bool CPU_RTC::InitializeWakeupTimer(void)
{
	// enable write access for RTC registers
	RTC_WriteAccess_Enable();

	// disable wake up timer (if enabled) to modify it
	RTC->CR &= ~(RTC_CR_WUTE);
	uint32_t tickstart = sysTickCnt;
	while((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_WUTWF);
			RTC_WriteAccess_Disable();
			return false;
		}
	}

	// set wake up reload counter
	RTC->WUTR = (rtcWut & 0x0000FFFFU);

	// select wake up clock to be ck_spre
	uint32_t val = RTC->CR;
	val &= RTC_CR_WUCKSEL;
	val |= RTC_CR_WUCKSEL_2;
	RTC->CR = val;

	// clear wake up flag
	RTC->ISR &= ~(RTC_ISR_WUTF);

	// enable wake up counter and interrupt
	RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

	// disable write access for RTC registers
	RTC_WriteAccess_Disable();

	// --- interrupt -------------------------------

	// enable EXTI line 20 ...
	EXTI->IMR |= EXTI_IMR_IM20;
	// ... with rising edge
	EXTI->RTSR |= EXTI_RTSR_TR20;

	// clear EXTI line 20 flag
	EXTI->PR |= EXTI_PR_PR20;

	rtcWakeUpFired = false;

	return true;
}

bool CPU_RTC::DisableRTC(void)
{
	bool res = true;

	NVIC_DisableIRQ(RTC_IRQn);

	EXTI->IMR &= ~(EXTI_IMR_IM20);

	res = DisableWakeupTimer();

	// no clock for RTC | RTC clock disable
	RCC->CSR &= ~(RCC_CSR_RTCSEL | RCC_CSR_RTCEN);

	// disable LSI
	RCC->CSR &= ~(RCC_CSR_LSION);
	// ... and wait until is ready
	uint32_t tickstart = sysTickCnt;
	while((RCC->CSR & RCC_CSR_LSIRDY) == RCC_CSR_LSIRDY){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_WaitLSI);
			return false;
		}
	}

	return res;
}

bool CPU_RTC::SetWakeupTimer(uint32_t wakeUpTicks)
{
	// enable write access for RTC registers
	RTC_WriteAccess_Enable();

	// disable wake up timer (if enabled) to modify it
	RTC->CR &= ~(RTC_CR_WUTE);
	uint32_t tickstart = sysTickCnt;
	while((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_WUTWF);
			RTC_WriteAccess_Disable();
			return false;
		}
	}

	// set wake up reload counter
	if(wakeUpTicks != 0) RTC->WUTR = (wakeUpTicks & 0x0000FFFFU);
	else                 RTC->WUTR = (rtcWut & 0x0000FFFFU);

	rtcWakeUpFired = false;

	// enable wake up counter and interrupt
	RTC->CR |= RTC_CR_WUTE | RTC_CR_WUTIE;

	// disable write access for RTC registers
	RTC_WriteAccess_Disable();

	return true;
}

bool CPU_RTC::DisableWakeupTimer(void)
{
	// enable write access for RTC registers
	RTC_WriteAccess_Enable();

	// disable wake up timer (if enabled) to modify it
	RTC->CR &= ~(RTC_CR_WUTE);
	uint32_t tickstart = sysTickCnt;
	while((RTC->ISR & RTC_ISR_WUTWF) != RTC_ISR_WUTWF){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_WUTWF);
			RTC_WriteAccess_Disable();
			return false;
		}
	}

	// disable wake up  interrupt
	RTC->CR &= ~(RTC_CR_WUTIE);

	// disable write access for RTC registers
	RTC_WriteAccess_Disable();

	return true;
}

// -----------------------------------------------------------------------------

void CPU_RTC::HandleInterrupt(void)
{
	if((RTC->ISR & RTC_ISR_WUTF) == RTC_ISR_WUTF) {
		RTC->ISR &= ~(RTC_ISR_WUTF); // clear wake up flag
		EXTI->PR |= EXTI_PR_PR20;    // clear EXTI line 20 flag

		rtcWakeUpFired = true;
	}
	else {
		// unhandled interrupt flag, disable RTC interrupt
		NVIC_DisableIRQ(RTC_IRQn);
	}
}

// -----------------------------------------------------------------------------

/**
 * ck_apre = RTC_clk / (PREDIVA + 1)
 * ck_spre = ck_apre / (PREDIVS + 1)
 *
 * PREDIVA = [0 ... 127]
 * PREDIVS = 15 bits in length
 *
 * Assuming RTC_clk = LSI = 37 kHz
 *
 * Case 1: PREDIVA = 127, PREDIVS = 288 -> 1,00022 Hz    (rtcPrescaler = 0x007F0120U)
 * Case 2: PREDIVA = 126, PREDIVS = 290 -> 1,00116 Hz    (rtcPrescaler = 0x007E0122U)
 * Case 3: PREDIVA = 125, PREDIVS = 293 -> 0,99881 Hz    (rtcPrescaler = 0x007D0125U)
 * Case 4: PREDIVA = 124, PREDIVS = 295 -> 1 Hz          (rtcPrescaler = 0x007C0127U)
 *
 * From RM0377: When both prescalers are used, it is recommended to configure
 * the asynchronous prescaler to a high value to minimize consumption.
 *
 * Case 1 is good enough because LSI is not very precise either.
 * The benefits of power consumption may be unmeasurable so I have selected case 4
 */
bool CPU_RTC::InitializePrescaler(void)
{
	// dot perform initialization if prescaler is already set
	if(RTC->PRER == rtcPrescaler)
		return true;

	// enable write access for RTC registers
	RTC_WriteAccess_Enable();

	// Enter RTC initialization mode (RTC->ISR |= RTC_ISR_INIT) but
	// the following statement also clears all other flags
	RTC->ISR = RTC_ISR_INIT;

	uint32_t tickstart = sysTickCnt;
	while((RTC->ISR & RTC_ISR_INITF) != RTC_ISR_INITF){
		if((sysTickCnt - tickstart) > rtcTimeout){
			hwLogger.AddEntry(FileID, LogCodeRTC, LogCodeRTC_INITF);
			RTC_WriteAccess_Disable();
			return false;
		}
	}

	// set prescaler for ck_spre ~ 1Hz
	RTC->PRER = rtcPrescaler;

	RTC->ISR &= ~(RTC_ISR_INIT);

	// disable write access for RTC registers
	RTC_WriteAccess_Disable();

	return true;
}

// -----------------------------------------------------------------------------

} /* namespace */

// -----------------------------------------------------------------------------

extern "C" void RTC_IRQHandler(void)
{
	PAx5::sRTC.HandleInterrupt();
}

// -----------------------------------------------------------------------------
