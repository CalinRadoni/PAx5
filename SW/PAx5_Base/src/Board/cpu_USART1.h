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

#ifndef cpu_USART1_H_
#define cpu_USART1_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t U1_RX_BUFF_LEN = 64;
const uint8_t U1_TX_BUFF_LEN = 64;

class CPU_USART1 {
public:
	CPU_USART1();
	virtual ~CPU_USART1();

	volatile uint8_t bufferRX[U1_RX_BUFF_LEN];
	volatile uint8_t buffRxIdx;
	volatile bool    dataReceived;
	volatile bool	 dataOverflow;

	volatile bool    intfError;

	/** Initialize this object's state */
	void Initialize(void);

	/** Configure the USART
	 *
	 * This function configure USART1 to 115200 bauds, 8N1, interrupt enabled
	 *
	 * Return false if no idle frame is detected within defined timeout.
	 */
	bool Configure(void);

	void InitTxData(void);
	void AddTxData(uint8_t);
	uint8_t TxDataAvailableSpace(void);
	void SendTxData(void);
	void SendTxDataAndWait(void);
	bool DataIsSent(void);

	void ClearRXBuffer(void);

	void HandleInterrupt(void);

protected:
	volatile uint8_t bufferTX[U1_RX_BUFF_LEN];
	volatile uint8_t buffTxLen;
	volatile uint8_t buffTxIdx;
	volatile bool dataSent;
};

extern CPU_USART1 sUSART;

} /* namespace */
#endif /* cpu_USART1_H_ */
