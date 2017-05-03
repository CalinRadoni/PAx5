/**
 * created 2016.04.23 by Calin Radoni
 */

#ifndef cpu_I2C_H_
#define cpu_I2C_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t I2CBufferLen = 16;

const uint8_t I2CStatus_OK       = 0;
const uint8_t I2CStatus_NACK     = 1;
const uint8_t I2CStatus_IntfErr  = 2;
const uint8_t I2CStatus_Timeout  = 4;
const uint8_t I2CStatus_Disabled = 8;

class CPU_I2C {
public:
	CPU_I2C();

	void Enable(void);
	void EnableAsSlave(uint8_t); // address
	void Disable(void);

	volatile uint8_t buffer[I2CBufferLen];
	volatile uint8_t buffLen;
	volatile uint8_t status;
	volatile bool transferDone;
	void Read(uint8_t); // slave address
	void Write(uint8_t); // slave address

	// for slave mode only, called after data has been received
	void onReceive(void(*)(uint8_t));

	// for slave mode only, called before sending data
	void onRequest(void(*)(void));

	void HandleI2CInt(void);

protected:
	volatile uint8_t buffIdx;
	uint8_t myAddress;
	volatile bool masterMode;
	volatile bool slaveReceiver;

    void (*callbackOnReceive)(uint8_t);
    void (*callbackOnRequest)(void);
};

extern CPU_I2C sI2C;

}
#endif /* cpu_I2C_H_ */
