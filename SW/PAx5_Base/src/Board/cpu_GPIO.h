/**
 * created 2017.04.23 by Calin Radoni
 */

#ifndef cpu_GPIO_H_
#define cpu_GPIO_H_

#include "MainBoard_Base.h"

namespace PAx5 {

enum class PinMode : uint8_t {
	Analog,     // analog mode
	Input,      // input, digital
	Output,     // output, push-pull
	OutputOD,   // output, open-drain
	Alternate,  // alternate function, push-pull
	AlternateOD // alternate function, open-drain
};

enum class PinPull : uint8_t {
	None, Up, Down, reserved
};

enum class PinSpeed : uint8_t {
	Low, Medium, High, VeryHigh
};

class CPU_GPIO_Definition {
public:
	CPU_GPIO_Definition() {};

	PinMode  mode        = PinMode::Analog;
	PinPull  pull        = PinPull::None;
	PinSpeed speed       = PinSpeed::Low;
	uint8_t  altFunction = 0;
};

class CPU_GPIO {
public:
	CPU_GPIO();
	virtual ~CPU_GPIO();

	/** Attach this object to a CPU pin
	 *
	 * @warning There are no checks to see if the pin is available on the current package or processor !
	 *
	 * @param pinID The ID of the pin as it is defined bellow. See PA0, PA!, ... definitions.
	 */
	void AttachToPin(uint8_t);

	/** Set the mode of the pin
	 *
	 * @return false if (GPIOx == nullptr) || (pinNumber > 0x0F).
	 */
	bool SetMode(const CPU_GPIO_Definition&);

	/** Get the mode of the pin */
	void GetMode(CPU_GPIO_Definition&);

	void SetPin(void);   ///< Set the pin to HIGH level
	void ResetPin(void); ///< Set the pin to LOW level
	bool GetPin(void);   ///< Get the input value of the pin

protected:
	GPIO_TypeDef* GPIOx = nullptr;
	uint32_t      pinNumber = 0;
};

/** Defines for ALL (even inexistent) pins */
const uint8_t PA0  = 0x00; const uint8_t PA1  = 0x01; const uint8_t PA2  = 0x02; const uint8_t PA3  = 0x03;
const uint8_t PA4  = 0x04; const uint8_t PA5  = 0x05; const uint8_t PA6  = 0x06; const uint8_t PA7  = 0x07;
const uint8_t PA8  = 0x08; const uint8_t PA9  = 0x09; const uint8_t PA10 = 0x0A; const uint8_t PA11 = 0x0B;
const uint8_t PA12 = 0x0C; const uint8_t PA13 = 0x0D; const uint8_t PA14 = 0x0E; const uint8_t PA15 = 0x0F;

const uint8_t PB0  = 0x10; const uint8_t PB1  = 0x11; const uint8_t PB2  = 0x12; const uint8_t PB3  = 0x13;
const uint8_t PB4  = 0x14; const uint8_t PB5  = 0x15; const uint8_t PB6  = 0x16; const uint8_t PB7  = 0x17;
const uint8_t PB8  = 0x18; const uint8_t PB9  = 0x19; const uint8_t PB10 = 0x1A; const uint8_t PB11 = 0x1B;
const uint8_t PB12 = 0x1C; const uint8_t PB13 = 0x1D; const uint8_t PB14 = 0x1E; const uint8_t PB15 = 0x1F;

const uint8_t PC0  = 0x20; const uint8_t PC1  = 0x21; const uint8_t PC2  = 0x22; const uint8_t PC3  = 0x23;
const uint8_t PC4  = 0x24; const uint8_t PC5  = 0x25; const uint8_t PC6  = 0x26; const uint8_t PC7  = 0x27;
const uint8_t PC8  = 0x28; const uint8_t PC9  = 0x29; const uint8_t PC10 = 0x2A; const uint8_t PC11 = 0x2B;
const uint8_t PC12 = 0x2C; const uint8_t PC13 = 0x2D; const uint8_t PC14 = 0x2E; const uint8_t PC15 = 0x2F;

} /* namespace */

#endif
