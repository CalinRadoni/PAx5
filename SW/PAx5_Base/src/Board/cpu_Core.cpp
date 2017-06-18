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

#include "cpu_Core.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

FILEID(2)

/**
 * associated data is:
 * LogCodeClock
 * - 1 for switch to MSI timeout
 * - 2 for switch to HSI timeout
 * - 3 for switch to PLL timeout
 * - 4 wait for MSI timeout
 * - 5 wait for HSI timeout
 * - 6 wait for PLL timeout
 * LogCodePwr
 * - 1 wait for WUF flag
 */
const uint8_t LogCodeClock = 1;
const uint8_t LogCodePwr = 2;

// -----------------------------------------------------------------------------

const uint32_t timeoutWait = 100U;
const uint32_t timeoutClockSwitch = 5000U;

// -----------------------------------------------------------------------------

CPU_Core sCPU;

// -----------------------------------------------------------------------------

CPU_Core::CPU_Core()
{
	frequency = Frequency::MSI_2M1;
	voltageRange = VoltageRange::VR2;
}

CPU_Core::~CPU_Core() { }

// -----------------------------------------------------------------------------

bool CPU_Core::SetFrequency(Frequency freqIn)
{
	bool res;

	switch(freqIn){
		case Frequency::MSI_65k:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_0);
			break;
		case Frequency::MSI_131k:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_1);
			break;
		case Frequency::MSI_262k:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_2);
			break;
		case Frequency::MSI_524k:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_3);
			break;
		case Frequency::MSI_1M:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_4);
			break;
		case Frequency::MSI_2M1:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_5);
			break;
		case Frequency::MSI_4M2:
			res = SetFrequency_MSI(RCC_ICSCR_MSIRANGE_6);
			break;
		case Frequency::HSI16_16M:
			res = SetFrequency_HSI16_16M();
			break;
		case Frequency::HSI16_32M:
			res = SetFrequency_HSI16_32M();
			break;
		default:
			res = false;
			break;
	}

	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000U);

	frequency = res ? freqIn : Frequency::Unknown;

	return res;
}

CPU_Core::Frequency CPU_Core::GetFrequency(void)
{
	return frequency;
}

uint8_t CPU_Core::Startup_SetClock(void)
{
	// Enable power interface clock
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	// Select voltage scale 1 (1.8V)
	while((PWR->CSR & PWR_CSR_VOSF) != 0) { /* wait */}
	PWR->CR = (PWR->CR & ~(PWR_CR_VOS)) | PWR_CR_VOS_0;
	while((PWR->CSR & PWR_CSR_VOSF) != 0) { /* wait */}

	voltageRange = VoltageRange::VR1;

	// Select flash to be 1 wait-state (required for 32MHZ on voltage scale 1)
	FLASH->ACR |= FLASH_ACR_LATENCY;

	// Enable HSI and HSI divided by 4 ...
	RCC->CR |= RCC_CR_HSION | RCC_CR_HSIDIVEN;
	// ... and wait for HSI to be ready
	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 5);
			return 1;
		}
	}

	// Enable PLL for HSI, multiply by 16 and divided by 2, then start PLL ...
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL16 | RCC_CFGR_PLLDIV2;
	RCC->CR |= RCC_CR_PLLON;
	// ... and wait for PLL to be ready
	tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_PLLRDY) == 0){
		if ((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 6);
			return 2;
		}
	}

	// Select PLL as system clock ...
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	// ... and wait for clock to switch on PLL
	tickstart = sysTickCnt;
	while((RCC->CFGR & RCC_CFGR_SWS_PLL) == 0){
		if((sysTickCnt - tickstart ) > timeoutClockSwitch){
			hwLogger.AddEntry(FileID, LogCodeClock, 3);
			return 3;
		}
	}

	frequency = Frequency::HSI16_32M;

	// disable MSI and do NOT wait for it
	RCC->CR &= ~(RCC_CR_MSION);

	return 0;
}

// -----------------------------------------------------------------------------

bool CPU_Core::NVM_WaitStateRequired(void)
{
	// see RM0377, 3.3 NVM functional description,
	// Table 15. Delays to memory access and number of wait states

	switch(frequency){
		case Frequency::HSI16_32M:
			return true;

		case Frequency::HSI16_16M:
			if(voltageRange==VoltageRange::VR1) return false;
			return true;

		default:
			return false;
	}

	return true;
}

void CPU_Core::EnableNVMWaitState(bool en)
{
	if(en) FLASH->ACR |= FLASH_ACR_LATENCY;
	else   FLASH->ACR &= ~(FLASH_ACR_LATENCY);
}

// -----------------------------------------------------------------------------

bool CPU_Core::AllowedVoltageRange(VoltageRange vr)
{
	// see RM0377, 3.3 NVM functional description,
	// Table 14. Link between master clock power range and frequencies

	switch(frequency){
		case Frequency::HSI16_32M:
			return (vr == VoltageRange::VR1);

		case Frequency::HSI16_16M:
			return (vr != VoltageRange::VR3);

		default:
			return true;
	}
}

void CPU_Core::SetVoltageRange(VoltageRange vr)
{
	while((PWR->CSR & PWR_CSR_VOSF) != 0) {
		/* wait for voltage regulator */
	}
	uint32_t val = PWR->CR;
	val &= ~(PWR_CR_VOS);
	switch(vr){
		case VoltageRange::VR1: val |= PWR_CR_VOS_0; break;
		case VoltageRange::VR2: val |= PWR_CR_VOS_1; break;
		case VoltageRange::VR3: val |= PWR_CR_VOS;   break;
	}
	PWR->CR = val;
	while((PWR->CSR & PWR_CSR_VOSF) != 0) {
		/* wait for voltage regulator */
	}

	voltageRange = vr;
}

// -----------------------------------------------------------------------------

bool CPU_Core::Clock_SwitchToMSI(void)
{
	// switch system clock to HSI
	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_SW);
	val |= RCC_CFGR_SW_MSI;
	RCC->CFGR = val;

	// wait for system clock to switch
	uint32_t tickstart = sysTickCnt;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_MSI){
		if((sysTickCnt - tickstart) > timeoutClockSwitch){
			hwLogger.AddEntry(FileID, LogCodeClock, 1);
			return false;
		}
	}

	return true;
}

bool CPU_Core::Clock_SwitchToHSI(void)
{
	// switch system clock to HSI
	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_SW);
	val |= RCC_CFGR_SW_HSI;
	RCC->CFGR = val;

	// wait for system clock to switch
	uint32_t tickstart = sysTickCnt;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI){
		if((sysTickCnt - tickstart) > timeoutClockSwitch){
			hwLogger.AddEntry(FileID, LogCodeClock, 2);
			return false;
		}
	}

	if((RCC->CR & RCC_CR_HSIDIVF) != 0) SysTick_Config(4000);
	else                                SysTick_Config(16000);

	return true;
}

bool CPU_Core::Clock_SwitchToPLL(void)
{
	// switch system clock to HSI
	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_SW);
	val |= RCC_CFGR_SW_PLL;
	RCC->CFGR = val;

	// wait for system clock to switch
	uint32_t tickstart = sysTickCnt;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){
		if((sysTickCnt - tickstart) > timeoutClockSwitch){
			hwLogger.AddEntry(FileID, LogCodeClock, 3);
			return false;
		}
	}

	return true;
}

bool CPU_Core::Clock_EnableMSI(void)
{
	RCC->CR |= RCC_CR_MSION;

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_MSIRDY) == 0){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 6);
			return false;
		}
	}
	return true;
}

bool CPU_Core::Clock_DisableMSI(void)
{
	RCC->CR &= ~(RCC_CR_MSION);

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_MSIRDY) != 0){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 6);
			return false;
		}
	}
	return true;
}

bool CPU_Core::Clock_EnableHSI(void)
{
	uint32_t val = RCC->CR;
	val &= ~(RCC_CR_HSION | RCC_CR_HSIDIVEN);
	val |= RCC_CR_HSION;
	RCC->CR = val;

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY)){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 5);
			return false;
		}
	}
	return true;
}

bool CPU_Core::Clock_EnableHSIdiv4(void)
{
	RCC->CR |= RCC_CR_HSION | RCC_CR_HSIDIVEN;

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 5);
			return false;
		}
	}
	return true;
}

bool CPU_Core::Clock_DisableHSI(void)
{
	RCC->CR &= ~(RCC_CR_HSION | RCC_CR_HSIDIVEN);

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & (RCC_CR_HSIRDY | RCC_CR_HSIDIVF)) != 0){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 5);
			return false;
		}
	}

	return true;
}

bool CPU_Core::Clock_EnablePLL(void)
{
	RCC->CR |= RCC_CR_PLLON;

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_PLLRDY) == 0){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 6);
			return false;
		}
	}
	return true;
}

bool CPU_Core::Clock_DisablePLL(void)
{
	RCC->CR &= ~(RCC_CR_PLLON);

	uint32_t tickstart = sysTickCnt;
	while((RCC->CR & RCC_CR_PLLRDY) != 0){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodeClock, 6);
			return false;
		}
	}
	return true;
}

bool CPU_Core::SetFrequency_MSI(uint32_t msiRange)
{
	// Enable power interface clock
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	uint32_t val = RCC->ICSCR;
	val &= ~(RCC_ICSCR_MSIRANGE);
	val |= msiRange;
	RCC->ICSCR = val;

	if(!Clock_EnableMSI()) return false;
	if(!Clock_SwitchToMSI()) return false;

	EnableNVMWaitState(false);
	SetVoltageRange(VoltageRange::VR3);

	if(!Clock_DisablePLL()) return false;
	if(!Clock_DisableHSI()) return false;

	return true;
}

bool CPU_Core::SetFrequency_HSI16_16M(void)
{
	// Enable power interface clock
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	if(!Clock_EnableHSIdiv4()) return false;
	if(!Clock_SwitchToHSI())   return false;
	if(!Clock_DisablePLL())    return false;

	EnableNVMWaitState(true);
	SetVoltageRange(VoltageRange::VR2);

	// Configure PLL with HSI, multiply by 8 and divided by 2
	// VR2 restricts the PLLVCO to max 48 MHz
	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_PLLSRC);
	val |= RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL8 | RCC_CFGR_PLLDIV2;
	RCC->CFGR = val;

	if(!Clock_EnablePLL())   return false;
	if(!Clock_SwitchToPLL()) return false;

	return true;
}

bool CPU_Core::SetFrequency_HSI16_32M(void)
{
	// Enable power interface clock
	RCC->APB1ENR |= (RCC_APB1ENR_PWREN);

	if(!Clock_EnableHSIdiv4()) return false;
	if(!Clock_SwitchToHSI())   return false;
	if(!Clock_DisablePLL())    return false;

	EnableNVMWaitState(true);
	SetVoltageRange(VoltageRange::VR1);

	// Configure PLL with HSI, multiply by 16 and divided by 2
	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_PLLSRC);
	val |= RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL16 | RCC_CFGR_PLLDIV2;
	RCC->CFGR = val;

	if(!Clock_EnablePLL())   return false;
	if(!Clock_SwitchToPLL()) return false;

	return true;
}

// -----------------------------------------------------------------------------

void CPU_Core::ConfigureSleep(bool voltageRegulatorSleep)
{
	/**
	 * set the power register. Info from RM0377
	 *
	 * PDDS Power-down deepsleep
	 * - 0: Enter Stop mode when the CPU enters Deepsleep
	 * - 1: Enter Standby mode when the CPU enters Deepsleep
	 *
	 * LPSDSR Low-power deepsleep/Sleep/Low-power run
	 * - In DeepSleep/Sleep modes: When this bit is set, the regulator switches in
	 * low-power mode when the CPU enters sleep or Deepsleep mode. The regulator
	 * goes back to Main mode when the CPU exits from these modes.
	 * - In Low-power run mode: When this bit is set, the regulator switches in
	 * low-power mode when the bit LPRUN is set. The regulator goes back to Main mode
	 * when the bit LPRUN is reset.
	 * 0: Voltage regulator on during Deepsleep/Sleep/Low-power run mode
	 * 1: Voltage regulator in low-power mode during Deepsleep/Sleep/Low-power run mode
	 */

	uint32_t val = PWR->CR;

	// clear PDDS and LPSDSR
	val &= ~(PWR_CR_PDDS | PWR_CR_LPSDSR);

	// set LPSDSR if requested
	if(voltageRegulatorSleep)
		val |= PWR_CR_LPSDSR;

	// set the mode
	PWR->CR = val;

	/**
	 * set the System Control register. Info from PM0223:
	 * SEVEONPEND  = 0: Only enabled interrupts or events can wakeup the processor, disabled interrupts are excluded
	 * SLEEPDEEP   = 0: Sleep, not Deep sleep
	 * SLEEPONEXIT = 0: Do not sleep when returning to Thread mode
	 */
	SCB->SCR = ~(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPONEXIT_Msk);
}

void CPU_Core::WakeupFromStopWithHSI(bool useHSI)
{
	uint32_t val = RCC->CFGR;

	if(useHSI) val |= RCC_CFGR_STOPWUCK;
	else       val &= ~(RCC_CFGR_STOPWUCK);

	RCC->CFGR = val;
}

void CPU_Core::EnterStop(void)
{
	/** see #ConfigureSleep for doc */

	uint32_t val = PWR->CR;

	// clear PDDS and LPSDSR
	val &= ~(PWR_CR_PDDS | PWR_CR_LPSDSR);

	// set the mode
	PWR->CR = val;

	/**
	 * set the System Control register. Info from PM0223:
	 * SEVEONPEND  = 0: Only enabled interrupts or events can wakeup the processor, disabled interrupts are excluded
	 * SLEEPDEEP   = 1: Deep sleep
	 * SLEEPONEXIT = 0: Do not sleep when returning to Thread mode
	 */
	val = SCB->SCR;
	val &= ~(SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPONEXIT_Msk);
	val |= SCB_SCR_SLEEPDEEP_Msk;
	SCB->SCR = val;

	// Errata 2.5.1 workaround, disable I2C1
	bool i2cON;
	if((I2C1->CR1 & I2C_CR1_PE) == I2C_CR1_PE){
		i2cON = true;
		I2C1->CR1 &= ~(I2C_CR1_PE);
	}
	else i2cON = false;

	/**
	 * Errata 2.1.9 workaround.
	 * The problem is only when executing WFI instruction from RAM.
	 * Setting RUN_PD bit requires unlocking first (see 3.6.4 in RM0377)
	 *
	 * FLASH->ACR |= FLASH_ACR_RUN_PD;
	 */

	// clear the wake up flag
	ClearWakeupFlag();

	__WFI();

	// restore previous frequency settings
	SetFrequency(frequency);

	// Errata 2.1.9 workaround
	// FLASH->ACR &= ~(FLASH_ACR_RUN_PD);

	// Errata 2.5.1 workaround
	if(i2cON)
		I2C1->CR1 |= I2C_CR1_PE;
}

void CPU_Core::EnterStandby(void)
{
	/** see #ConfigureSleep for doc */

	PWR->CR |= PWR_CR_PDDS;

	uint32_t val = SCB->SCR;
	val &= ~(SCB_SCR_SEVONPEND_Msk | SCB_SCR_SLEEPONEXIT_Msk);
	val |= SCB_SCR_SLEEPDEEP_Msk;
	SCB->SCR = val;

	ClearWakeupFlag();

	__WFI();
}

void CPU_Core::ClearStanbyFlag(void)
{
	PWR->CR |= PWR_CR_CSBF;
}

void CPU_Core::ClearWakeupFlag(void)
{
	PWR->CR |= PWR_CR_CWUF;

	uint32_t tickstart = sysTickCnt;
	while((PWR->CSR & PWR_CSR_WUF) != PWR_CSR_WUF){
		if((sysTickCnt - tickstart) > timeoutWait){
			hwLogger.AddEntry(FileID, LogCodePwr, 1);
			return;
		}
	}
}

bool CPU_Core::IsVoltageRegulatorInLowPowerMode(void)
{
	return ((PWR->CSR & PWR_CSR_REGLPF) == PWR_CSR_REGLPF);
}

// -----------------------------------------------------------------------------

bool CPU_Core::Clock_CleanUp(void)
{
	bool res = true;

	uint32_t val = RCC->CFGR;
	val &= ~(RCC_CFGR_SW);
	switch(val){
		case RCC_CFGR_SWS_MSI:
			// disable PLL and HSI
			res = res && Clock_DisablePLL();
			res = res && Clock_DisableHSI();
			break;
		case RCC_CFGR_SWS_HSI:
			// disable PLL and MSI
			res = res && Clock_DisablePLL();
			res = res && Clock_DisableMSI();
			break;
		case RCC_CFGR_SWS_PLL:
			// disable MSI
			res = res && Clock_DisableMSI();
			break;
		default:
			res = false;
			break;
	}
	return true;
}

// -----------------------------------------------------------------------------
}
