/**
 * created 2016.04.23 by Calin Radoni
 */

#ifndef cpu_I2C_H_
#define cpu_I2C_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define I2C_BUFF_LEN 16

#define I2C_STATE_OK       0
#define I2C_STATE_NACK     1
#define I2C_STATE_IntfErr  2
#define I2C_STATE_Timeout  4
#define I2C_STATE_Disabled 8

class CPU_I2C {
protected:
	volatile uint8_t buffIdx;
	uint8_t myAddress;
	volatile bool masterMode;
	volatile bool slaveReceiver;

    void (*callbackOnReceive)(uint8_t);
    void (*callbackOnRequest)(void);

public:
	CPU_I2C();

	void Enable(void);
	void EnableAsSlave(uint8_t); // address
	void Disable(void);

	volatile uint8_t buffer[I2C_BUFF_LEN];
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
};

extern CPU_I2C sI2C;

}
#endif /* cpu_I2C_H_ */
