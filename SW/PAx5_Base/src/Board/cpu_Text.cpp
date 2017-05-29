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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "cpu_Text.h"
#include "cpu_USART1.h"

namespace PAx5 {
// -----------------------------------------------------------------------------

CPU_TextOutput sTextOutput;

#define PAD_BINARY  '0'
#define PAD_HEX     '0'
#define PAD_Default ' '

// -----------------------------------------------------------------------------

CPU_TextOutput::CPU_TextOutput() {
}

CPU_TextOutput::~CPU_TextOutput() {
}

// -----------------------------------------------------------------------------

void CPU_TextOutput::InitBuffer(void)
{
	sUSART.InitTxData();
}

void CPU_TextOutput::Flush(void)
{
	sUSART.SendTxDataAndWait();
}

// -----------------------------------------------------------------------------

#define ConvBufSize (8 * sizeof(unsigned int))

void CPU_TextOutput::ConvertAndOutputUtoD(unsigned int d, unsigned int baseIn, unsigned char minLength)
{
	unsigned char ch, idx;
	unsigned int base;

	// if base==2 buffer length should be 8 * sizeof(unsigned int) bytes
	char buf[ConvBufSize];

	base = baseIn;
	if(base < 2)
		base = 2;

	idx = 0;
	do {
		ch = d % base;
		d /= base;
		if(ch > 9) buf[idx] = (ch - 10) + 'A';
		else       buf[idx] = ch + '0';
		idx++;
	} while(d > 0);

	if(minLength > 1){
		while((idx < minLength) && (idx < ConvBufSize)){
			switch(baseIn){
			case  2: buf[idx] = PAD_BINARY;  break;
			case 16: buf[idx] = PAD_HEX;     break;
			default: buf[idx] = PAD_Default; break;
			}
			idx++;
		}
	}

	while(idx > 0){
		idx--;
		sUSART.AddTxData(buf[idx]);
	}
}

void CPU_TextOutput::FormatAndOutputString(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char ch;
	bool formatSpec;
	unsigned char minLen;

	minLen = 0; // just to remove a warning
	formatSpec = false;
	while(*fmt) {
		if (*fmt == '%') {
			formatSpec = true;
			minLen = 0;
		}
		else {
			if(!formatSpec) sUSART.AddTxData(*fmt);
			else{
				formatSpec = false;
				switch(*fmt){
				case 'c':
					sUSART.AddTxData((uint8_t)va_arg(va, int));
					break;
				case 's':
					{
						char * arg = va_arg(va, char *);
						while (*arg)
							sUSART.AddTxData(*arg++);
					}
					break;
				case '%':
					sUSART.AddTxData('%');
					break;
				case 'b':
					ConvertAndOutputUtoD(va_arg(va, int), 2, minLen);
					break;
				case 'd':
				case 'i':
					{
						signed int val = va_arg(va, signed int);
						if (val < 0) {
							val = 0 - val;
							sUSART.AddTxData('-');
						}
						ConvertAndOutputUtoD((unsigned int)val, 10, minLen);
					}
					break;
				case 'u':
					ConvertAndOutputUtoD(va_arg(va, unsigned int), 10, minLen);
					break;
				case 'x':
					ConvertAndOutputUtoD(va_arg(va, int), 16, minLen);
					break;
				default:
					ch = *fmt;
					if((ch>='0') && (ch<='9')){
						minLen *= 10;
						minLen += (ch - '0');
						formatSpec = true;
					}
					else{
						sUSART.AddTxData('%');
						sUSART.AddTxData(ch);
					}
					break;
				}
			}
		}
		fmt++;
	}

	va_end(va);
}

void CPU_TextOutput::OutputBuffer(uint8_t* buffIn, uint8_t lenIn)
{
	for(uint8_t i = 0; i < lenIn; i++)
		sUSART.AddTxData(buffIn[i]);
}

void CPU_TextOutput::OutputBuffer(volatile uint8_t* buffIn, uint8_t lenIn)
{
	for(uint8_t i = 0; i < lenIn; i++)
		sUSART.AddTxData(buffIn[i]);
}

// -----------------------------------------------------------------------------

} /* namespace */
