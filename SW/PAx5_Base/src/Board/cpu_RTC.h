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

#ifndef cpu_RTC_H_
#define cpu_RTC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_RTC {
public:
	CPU_RTC();
	virtual ~CPU_RTC();

	/**
	 * \brief Initialize RTC module
	 *
	 * \details This function performs the following operations:
	 * - turn on LSI and set it as the clock for RTC
	 * - initialize RTC's prescaler
	 * - enable RTC interrupt
	 */
	bool InitializeRTC(void);

	/**
	 * \brief Disable RTC
	 *
	 * \details Disables:
	 * - RTC interrupt
	 * - wake up timer
	 * - RTC clock
	 * - LSI clock
	 *
	 * Call #InitializeRTC to enable it again.
	 */
	bool DisableRTC(void);

	/**
	 * \brief Initialize the wake up timer
	 *
	 * \brief This function is called by #InitializeRTC function.
	 * This function:
	 * - set wake up timer to 10 minutes
	 * - enable wake up timer and its interrupt
	 */
	bool InitializeWakeupTimer(void);

	/**
	 * \brief Reset the wake up timer
	 *
	 * \details Use the following formula to compute required ticks:
	 *
	 * Wake up ticks = time[s] - 1
	 *
	 * If #wakeUpTics is 0, initialize the wake up reload register with the value defined by #rtcWut.
	 *
	 * This function also clears #rtcWakeUpFired variable.
	 *
	 * \note Only the lower 16 bits of #wakeUpTics are used.
	 */
	bool SetWakeupTimer(uint32_t wakeUpTicks);

	/**
	 * Disables the wake up timer.
	 * Call #Initialize
	 *
	 * \warning The wake up timer should be enabled to wake up the CPU from Stop or Standby modes.
	 */
	bool DisableWakeupTimer(void);

	void DisableRTCInterrupt(void);

	volatile bool rtcWakeUpFired;
	void HandleInterrupt(void);

protected:
	/**
	 * \brief Initialize the RTC prescaler.
	 *
	 * \details The prescaler is initialized for a low power consumption.
	 * The asynchronous prescaler is set to 124, and the synchronous prescaler to 295.
	 * For 37 kHz (the frequency of LSI) the resulting frequency is 1 Hz.
	 *
	 * See the definition of the function for other values.
	 */
	bool InitializePrescaler(void);
};

extern CPU_RTC sRTC;

} /* namespace */
#endif
