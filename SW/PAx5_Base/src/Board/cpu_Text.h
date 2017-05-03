/**
 * created 2016.04.23 by Calin Radoni
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
