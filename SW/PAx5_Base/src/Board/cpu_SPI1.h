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

#ifndef cpu_SPI1_H_
#define cpu_SPI1_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t SPISlave_None   = 0;
const uint8_t SPISlave_Memory = 1;
const uint8_t SPISlave_Radio  = 2;

class CPU_SPI1 {
public:
	CPU_SPI1();
	virtual ~CPU_SPI1();

	volatile bool transferDone;
	volatile bool intfError;

	void Initialize(void);
	volatile uint8_t clockDivider;
	void Configure(void);

	void SendCommandAndWait(uint8_t, uint8_t); // command, slave ID
	uint8_t SendCmdPlusAndWait(uint8_t, uint8_t, uint8_t); // command, data, slave ID
	void SendBufferAndWait(volatile uint8_t*, uint16_t, uint8_t); // buffer, size, slave ID

	volatile bool defaultSPISettings;

	/** Configure for using RAM_... functions
	 *
	 * A successfull call of this function will set SPI to 'dumb' mode, i.e. no interrupts and no DMA.
	 *
	 * @warning For this function to return true, transferDone and defaultSPISettings must be true.
	 */
	bool ConfigureForRAM(void);

	/** RAM version of the SendCommandAndWait function
	 *
	 * @warning This functions disables interrupts temporary
	 *
	 * @param command
	 * @param data
	 * @param slaveID
	 * @return received byte
	 */
	__RAMFUNC uint8_t RAM_SendCmdPlusAndWait(uint8_t, uint8_t, uint8_t);

	/** RAM version of the SendBufferAndWait function
	 *
	 * @warning This functions disables interrupts temporary
	 *
	 * @param buffer
	 * @param bufferSize
	 * @param slaveID
	 */
	__RAMFUNC void RAM_SendBufferAndWait(volatile uint8_t*, uint16_t, uint8_t);

	void HandleDMAInt(void);

protected:
	volatile uint8_t smallBuffer[2];

	volatile uint8_t spiSlave;
	volatile bool readyToSend;
};

extern CPU_SPI1 sSPI;

} /* namespace */
#endif /* cpu_SPI1_H_ */
