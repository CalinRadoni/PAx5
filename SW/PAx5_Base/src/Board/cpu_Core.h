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

#ifndef cpu_CORE_H_
#define cpu_CORE_H_

#include <MainBoard_Base.h>

#include <cpu_Clocks.h>

namespace PAx5 {

class CPU_Core {
public:
	CPU_Core();
	virtual ~CPU_Core();

	/**
	 * When adding an entry in this table modify, if needed, the following functions:
	 * - SetFrequency
	 * - NVM_WaitStateRequired
	 * - AllowedVoltageRange
	 *
	 */
	enum class Frequency : uint8_t {
		MSI_65k,       //  65.536 kHz, MSI,       VR3, NVM 0 wait-state
		MSI_131k,      // 131.072 kHz, MSI,       VR3, NVM 0 wait-state
		MSI_262k,      // 262.144 kHz, MSI,       VR3, NVM 0 wait-state
		MSI_524k,      // 524.288 kHz, MSI,       VR3, NVM 0 wait-state
		MSI_1M,        // 1.048 MHz,   MSI,       VR3, NVM 0 wait-state
		MSI_2M1,       // 2.097 MHz,   MSI,       VR3, NVM 0 wait-state
		MSI_4M2,       // 4.194 MHz,   MSI,       VR3, NVM 0 wait-state
		HSI16_16M,     // 16 MHz,      HSI,       VR2, NVM 1 wait-state
		HSI16_16M_PLL, // 16 MHz,      HSI + PLL, VR2, NVM 1 wait-state
		HSI16_32M_PLL, // 32 MHz,      HSI + PLL, VR1, NVM 1 wait-state
		Unknown
	};

	enum class VoltageRange : uint8_t {
		VR1, // 1.8 V
		VR2, // 1.5 V
		VR3  // 1.2 V
	};

	/**
	 *  Usage:
	 * \code{.cpp}
	 * cpuClocks.clockSYS = SystemCoreClock;
	 * cpuClocks.Startup_SetValues();
	 * ...
	 * cpuClocks.clockSYS = SystemCoreClock;
	 * cpuClocks.UpdateClockValues();
	 * \endcode
	 */
	CPU_Clocks cpuClocks;

	/**
	 * \brief Created to be called after startup, configures system clock to 32 MHz using HSI
	 *
	 * \details This function:
	 * - enable power interface clock
	 * - set voltage regulator's range to VR1
	 * - enable 1 wait-state latency for NVM
	 * - enable HSI
	 * - enable PLL for HSI, multiply with 4 and divide with 2
	 * - select PLL as system clock.
	 *
	 * \note Even if the function fails, #voltageRange and #frequency have the correct values.
	 *
	 * \return 0 OK
	 *         1 HSI Timeout
	 *         2 PLL Timeout
	 *         3 Clock switch timeout
	 */
	uint8_t Startup_SetClock(void);

	/**
	 * \brief Change the system clock and frequency
	 *
	 * \details After startup, MSI at 2.1MHz is used as system clock.
	 * This function changes the frequency and clock source and set:
	 * - the internal voltage regulator
	 * - the required wait states for NVM
	 * - the system timer
	 *
	 * Even if this function returns false, it still call the #SystemCoreClockUpdate and #SysTick_Config
	 * functions to set the #SystemCoreClock variable and the System Timer based on current, real, values.
	 * When the return value is false the frequency is set to Frequency::Unknown.
	 *
	 * \note Changing the frequency to MSI_xxx will disable HSI and PLL.
	 *
	 * \note This function does NOT call the #Clock_CleanUp function. Call that function if you want.
	 *
	 * \warning Changing the system clock's frequency affects at least:
	 * - UART, I2C and SPI modules
	 * - Timers
	 * To use them you may need to reconfigure them.
	 */
	bool SetFrequency(Frequency);

	/**
	 * \brief Returns the frequency set by #SetFrequency function
	 */
	Frequency GetFrequency(void);

	/**
	 * \brief Prepare the core for entering the Sleep mode
	 *
	 * \details In Sleep mode, all I/O pins keep the same state as in Run mode.
	 * CPU clock is off but the mode has no effect on other clocks or analog clock sources.
	 * If voltage regulator is in sleep mode the wake up time increases.
	 *
	 * Send Event on Pending is cleared so only enabled interrupts or events can
	 * wakeup the processor, disabled interrupts are excluded.
	 *
	 * NO sleep-on-exit when returning from Handler mode to Thread mode.
	 *
	 * \param voltageRegulatorSleep Puts the voltage regulator in sleep mode if true.
	 *
	 */
	void ConfigureSleep(bool voltageRegulatorSleep);

	/**
	 * Select if HSI is the clock for wake up from Stop mode or the default, MSI, value.
	 */
	void WakeupFromStopWithHSI(bool);

	/**
	 * \brief Enters the Stop mode
	 *
	 * \details In Stop mode, all I/O pins keep the same state as in Run mode.
	 *
	 * After resuming Stop mode the frequency is set to previous value.
	 *
	 * \warning The interrupt processed while the frequency is not the desired one
	 *          may cause unpredictable behaviour ! PM0223 recommends to use PRIMASK.PM
	 *          to permanently disable interrupts but a new clock setting procedure
	 *          (without current procedure for wait states) must be created and this
	 *          can cause more problems.
	 *
	 * There are some silicon problems with the Stop mode. See STM32L051x6/8 Errata sheet
	 * and the function definition.
	 *
	 * Exiting the Stop mode resume program execution.
	 *
	 * \note 1. This function calls #Clock_CleanUp function after exiting STOP mode.
	 *
	 * \note 2. As errata 2.1.10 workaround, the voltage regulator will NOT be put in low power mode.
	 *       This also allows I2C and USART to be used as wake up sources (errata 2.1.11 workaround).
	 *
	 * \note 3. As errata 2.5.1 workaround, I2C1 is disabled before entering Stop mode.
	 *       After exiting Stop mode I2C state will be restored.
	 *
	 * \warning Do not call this function from RAM, errata 2.1.9 not corrected ! The workaround
	 *          requires more code that may be useful for this function.
	 */
	void EnterStop(void);

	/**
	 * \brief Enters the Standby mode
	 *
	 * \details In Standby mode, all I/O pins are high impedance except the reset pin.
	 * Exiting Standby mode is like a CPU reset.
	 *
	 * Usage to wake up after 5 minutes:
	 * \code{.cpp}
	 * sRTC.InitializeRTC();
	 * ...
	 * if(sRTC.SetWakeupTimer(299))
	 *     sCPU.EnterStandby();
	 * \endcode
	 */
	void EnterStandby(void);

	void ClearStanbyFlag(void);
	void ClearWakeupFlag(void);

	/**
	 * \brief Check if the voltage regulator is still in low-power mode
	 *
	 * From RM0377, Power control/status register, bit REGLPF:
	 * Polling is recommended to wait for the regulator Main mode.
	 */
	bool IsVoltageRegulatorInLowPowerMode(void);

	/**
	 * \brief Disable oscillators or PLL if not used by system clock
	 *
	 * \details The system clock can use MSI, HSI, HSE or PLL.
	 *          The system does not use HSE.
	 *          This function disable oscillators or PLL not used by
	 *          current configuration readed from RCC_CFGR register.
	 */
	bool Clock_CleanUp(void);

protected:
	Frequency frequency;
	VoltageRange voltageRange;

	/**
	 * Returns true if wait-state is needed for current
	 * #frequency and #voltageRange
	 */
	bool NVM_WaitStateRequired(void);

	void EnableNVMWaitState(bool);

	/**
	 * Check if the voltage range passed as parameter is allowed
	 * for current #frequency
	 */
	bool AllowedVoltageRange(VoltageRange);

	void SetVoltageRange(VoltageRange);

	/** Switch the system clock to MSI */
	bool Clock_SwitchToMSI(void);

	/**
	 * Switch the system clock to HSI16 and calls
	 * SysTick_Config to update the System Timer and its interrupt
	 */
	bool Clock_SwitchToHSI(void);

	/** Switch the system clock to PLL */
	bool Clock_SwitchToPLL(void);

	bool Clock_EnableMSI(void);
	bool Clock_DisableMSI(void);
	bool Clock_EnableHSI(void);
	bool Clock_EnableHSIdiv4(void);
	bool Clock_DisableHSI(void);
	bool Clock_EnablePLL(void);
	bool Clock_DisablePLL(void);

	bool SetFrequency_MSI(uint32_t);
	bool SetFrequency_HSI16_16M(void);
	bool SetFrequency_HSI16_16M_PLL(void);
	bool SetFrequency_HSI16_32M_PLL(void);
};

extern CPU_Core sCPU;

} /* namespace */
#endif
