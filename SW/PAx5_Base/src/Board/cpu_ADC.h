/**
 * created 2016.03.27 by Calin Radoni
 */

#ifndef cpu_ADC_H_
#define cpu_ADC_H_

#include "MainBoard_Base.h"

namespace PAx5 {

#define A0       ((uint8_t)0)
#define A1       ((uint8_t)1)
#define A2       ((uint8_t)2)
#define A3       ((uint8_t)3)
#define ATemp    ((uint8_t)18)
#define AVrefInt ((uint8_t)17)

class CPU_ADC {
protected:
	uint32_t VddA;

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
};

extern CPU_ADC sADC;

}
#endif /* cpu_ADC_H_ */
