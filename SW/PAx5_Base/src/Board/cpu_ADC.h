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

#ifndef cpu_ADC_H_
#define cpu_ADC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t A0       = 0;
const uint8_t A1       = 1;
const uint8_t A2       = 2;
const uint8_t A3       = 3;
const uint8_t ATemp    = 18;
const uint8_t AVrefInt = 17;

class CPU_ADC {
public:
	CPU_ADC();

	/**
	 * \brief Aquired data
	 *
	 * \details #ReadChannel puts the result in this variable.
	 * #ConvertDataToVoltage and #ConvertDataToTemperature converts the value stored
	 * in this variable and puts the result back here.
	 */
	uint16_t data;

	/**
	 * \brief Oversampling ratio
	 *
	 * \details Oversampling ratio:
	 * - 0 =   2x, shift 1-bit
	 * - 1 =   4x, shift 2-bit
	 * - 2 =   8x, shift 3-bit
	 * - 3 =  16x, shift 4-bit
	 * - 4 =  32x, shift 5-bit
	 * - 5 =  64x, shift 6-bit
	 * - 6 = 128x, shift 7-bit
	 * - 7 = 256x, shift 8-bit
	 * - other values = oversampling disabled
	 *
	 * This variable is used by the ReadChannel function.
	 */
	uint8_t oversamplingRatio;

	/**
	 * \brief Configure and enable ADC
	 *
	 * ADCCLK is HSI16 which can be 16 or 4 MHz depending of HSI divider
	 * ADCCLK can be divided with ADC->CCR PRESC[3:0]
	 *
	 * Other settings:
	 * - Oversampler disabled
	 * - Temperature sensor disabled
	 * - VREFINT disabled
	 *
	 * \note The ADC interrupts are NOT used and will be disabled by this function
	 *
	 * \param clockPrescaler 0 ... 11 for 1, 2, 4, 6, 8, 10, 12, 16, 32, 64, 128, 256 division
	 * \param inputClockFrequency the frequency of input clock. Required to set Low Frequency mode if needed.
	 * \param samplingTime 0 ... 7 for 1.5, 3.5, 7.5, 12.5, 19.5, 39.5, 79.5, 160.5 ADC clock cycles
	 *
	 * \note PCLK removed because I did not have uses to justify supplementary code for this mode !
	 *
	 * \warning CPU_EntropyADC uses the same ADC but with another settings. Call this function again
	 *          after using CPU_EntropyADC !
	 *
	 * \note Conversion time calculation: conversion time = tSAR + tSMPL
	 *   RM0377 13.5.2 Programmable resolution (RES) - fast conversion mode
	 *     tSAR = 12.5 / 11.5 / 9.5 / 7.5 ADC clock cycles for a resolution of 12 / 10 / 8 / 6 bits
	 *     tSAR is in ADC->CFGR1 bits RES[1:0]
	 *   RM0377 13.12.6 ADC sampling time register (ADC_SMPR)
	 *     tSMPL = 1.5 / 3.5 / 7.5 / 12.5 / 19.5 / 39.5 / 79.5 / 160.5 ADC clock cycles depending on ADC->SMPR bits SMP[2:0]
	 *
	 * \return false if #clockPrescaler or #samplingTime are out of their allowed range
	 */
	bool Enable(uint8_t clockPrescaler, uint32_t inputClockFrequency, uint8_t samplingTime);

	/**
	 * \brief Read an analog channel
	 *
	 * \details Workflow:
	 * - enable Temperature sensor or VREFINT if requested
	 * - compute oversampling parameters
	 * - read the channel and discard the result (RM0377 / datasheet errata)
	 * - apply oversampling parameters
	 * - read the channel
	 * - disable Temperature sensor or VREFINT if previously enabled
	 *
	 * \param uint8_t is the channel. See Axxx defines at this file's top
	 *
	 * \return Readed value is stored in the #data variable
	 */
	void ReadChannel(uint8_t);

	void Disable(void);

	/**
	 * \brief Reads VddA
	 *
	 * \note This function should be used before #ConvertDataToVoltage and #ConvertDataToTemperature
	 * functions for a correct conversion.
	 */
	void ReadVDDA(void);

	/**
	 * \brief Converts the last read ADC value to actual voltage
	 *
	 * \details The converted value is put back in #data after multiplication with 100 (data = 100 * actual_voltage).
	 *
	 * \warning For a correct conversion ReadVDDA must be called first !
	 */
	void ConvertDataToVoltage(void);

	/**
	 * \brief Converts the last readed data to a temperature value.
	 *
	 * \details Converts the value previously readed with #ReadChannel(ATemp) function
	 *
	 * \warning For a correct conversion ReadVDDA must be called first !
	 */
	int8_t ConvertDataToTemperature(void);

	/** GetCx where used in debugging, and are used by BoardTester */
	void GetC1(void);
	void GetC2(void);
	void GetC3(void);

protected:
	uint32_t VddA;
};

extern CPU_ADC sADC;

}
#endif /* cpu_ADC_H_ */
