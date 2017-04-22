/**
 * created 2017.01.17 by Calin Radoni
 */

#ifndef cpu_DMA_H_
#define cpu_DMA_H_

#include "MainBoard_Base.h"

namespace PAx5 {

class CPU_DMA {
public:
	CPU_DMA();

	// DMA1 channel 1 is used for DMA memory-to-memory transfer

	void M2M_Initialize(void);
	void M2M_Copy(uint32_t srcAddr, uint32_t dstAddr, uint32_t dataLen);
	void M2M_CleanUp(void);

	/** Check if the transfer is done
	 *
	 * @return 0 for transfer in progress or not done
	 * @return 1 for transfer completed
	 * @return 2 for transfer error
	 */
	uint8_t M2M_TransferStatus(void);
};

extern CPU_DMA sDMA;

} /* namespace */

#endif
