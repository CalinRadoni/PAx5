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

#ifndef cpu_TEXT_H_
#define cpu_TEXT_H_

#include "MainBoard_Base.h"

namespace PAx5 {

/** Outputs formatted text to USART */
class CPU_TextOutput {
public:
	CPU_TextOutput();
	virtual ~CPU_TextOutput();

	/** Initialize output Buffer
	 *
	 * InitBuffer sets the length of USART's TX buffer to 0
	 */
	void InitBuffer(void);

	/** Flush output buffer
	 *
	 * Flush is waiting for USART to send the content of its TX buffer
	 */
	void Flush(void);

	/** Format and output a string
	 *
	 * FormatAndOutputString formats a string in a way similar to standard printf behaviour.
	 * The output is sent to USART.
	 * Format options are:
	 * - c    char
	 * - d,i  integer
	 * - s    string, NULL terminated
	 * - u    unsigned integer
	 * - x,X  unsigned integer - hexadecimal
	 * - b    unsigned integer - binary
	 * Before b, u, i and x the minimum output length can be specified.
	 * Minimum output length is limited to 255. Other values will overflow.
	 * Padding is done with '0' for binary and hexadecimal unsigned integer and with space for others.
	 *
	 * @code{.cpp}
	 * sTextOutput.FormatAndOutputString("Text and numbers: %8b %4d %d 0x%4x %c\r\n", 11, 12, 13, 14, 37);
	 * @endcode
	 */
	void FormatAndOutputString(const char *fmt, ...);

	/** Output a buffer
	 *
	 * OutputBuffer send the buffer "as is" to the USART
	 */
	void OutputBuffer(uint8_t*, uint8_t);
	void OutputBuffer(volatile uint8_t*, uint8_t);

protected:
	void ConvertAndOutputUtoD(unsigned int d, unsigned int base, unsigned char minLength = 0);
};

extern CPU_TextOutput sTextOutput;

} /* namespace */
#endif /* cpu_TEXT_H_ */
