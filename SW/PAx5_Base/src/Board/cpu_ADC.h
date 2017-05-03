/**
 * created 2016.03.27 by Calin Radoni
 */

#ifndef cpu_ADC_H_
#define cpu_ADC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t A0       = 0;
const uint8_t A1       = 1;
const uint8_t A2       = 2;
const uint8_t A3       = 3;
const uint8_t ATemp    = 18;
const uint8_t AVrefInt = 17;

class CPU_ADC {
public:
	CPU_ADC();

	uint16_t data;             ///< readed data

	uint8_t oversamplingRatio; ///< useable values 0 ... 7

	void Enable(void);

	/** Read an analog channel
	 *
	 * @param uint8_t is the channel. See Axxx defines at this file's top
	 * @return Readed value is stored in the data variable
	 */
	void ReadChannel(uint8_t); // channel
	void Disable(void);

	/** Reads VddA
	 *
	 * This function should be used before ConvertDataToVoltage and ConvertDataToTemperature
	 * functions for a correct conversion.
	 */
	void ReadVDDA(void);

	/** Converts the last read ADC value to actual voltage
	 *
	 * The converted value is put back in 'data' after multiplication with 100 (data = 100 * actual_voltage).
	 *
	 * @warning For a correct conversion ReadVDDA must be called first !
	 */
	void ConvertDataToVoltage(void);

	/** Converts the last readed data to a temperature value.
	 *
	 * It is usefull to convert the value readed with ReadChannel(ATemp)
	 *
	 * @warning For a correct conversion ReadVDDA must be called first !
	 */
	int8_t ConvertDataToTemperature(void); // Call ReadVDDA first !

	/** GetCx where used in debugging, and are used by BoardTester */
	void GetC1(void);
	void GetC2(void);
	void GetC3(void);

protected:
	uint32_t VddA;
};

extern CPU_ADC sADC;

}
#endif /* cpu_ADC_H_ */
