/**
 * created 2016.02.20 by Calin Radoni
 */

#ifndef cpu_USART1_H_
#define cpu_USART1_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define U1_RX_BUFF_LEN 64
#define U1_TX_BUFF_LEN 64

class CPU_USART1 {
protected:
	volatile uint8_t bufferTX[U1_RX_BUFF_LEN];
	volatile uint8_t buffTxLen;
	volatile uint8_t buffTxIdx;
	volatile bool dataSent;

public:
	CPU_USART1();
	virtual ~CPU_USART1();

	volatile uint8_t bufferRX[U1_RX_BUFF_LEN];
	volatile uint8_t buffRxIdx;
	volatile bool    dataReceived;
	volatile bool	 dataOverflow;

	volatile bool    intfError;

	void Initialize(void);
	bool Configure(void);

	void InitTxData(void);
	void AddTxData(uint8_t);
	uint8_t TxDataAvailableSpace(void);
	void SendTxData(void);
	void SendTxDataAndWait(void);
	bool DataIsSent(void);

	void ClearRXBuffer(void);

	void HandleInterrupt(void);
};

extern CPU_USART1 sUSART;

} /* namespace */
#endif /* cpu_USART1_H_ */
