/**
 * created 2016.02.20 by Calin Radoni
 */

#ifndef cpu_SPI1_H_
#define cpu_SPI1_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define SPI_SLAVE_NONE   0
#define SPI_SLAVE_Memory 1
#define SPI_SLAVE_Radio  2

class CPU_SPI1 {
protected:
	volatile uint8_t smallBuffer[2];

	volatile uint8_t spiSlave;
	volatile bool readyToSend;

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
};

extern CPU_SPI1 sSPI;

} /* namespace */
#endif /* cpu_SPI1_H_ */
