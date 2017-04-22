/**
 * created 2016.04.25 by Calin Radoni
 */

#ifndef ext_HIHSENSOR_H_
#define ext_HIHSENSOR_H_

#include "cpu_I2C.h"

namespace PAx5 {

#define HIH_DATA_OK      0
#define HIH_DATA_Stale   1
#define HIH_DATA_NACK    2
#define HIH_DATA_IntfErr 3

class EXT_HIHSensor {
protected:
	//uint16_t rawH, rawT;

public:
	EXT_HIHSensor();
	virtual ~EXT_HIHSensor();

	uint16_t rawH, rawT;

	/** @brief Begin measurement cycle
	 *
	 * Call this function to start a measurement cycle.
	 * The measurement cycle duration is typically 36.65 ms for temperature and humidity readings.
	 *
	 * @return HIH_DATA_OK is sensor acknowledged the packet and HIH_DATA_IntfErr is sensor not present.
	 */
	uint8_t ReadInit(void);

	/** @brief Read measured data
	 *
	 * @return HIH_DATA_OK if new data was read
	 * @return HIH_DATA_Stale if old data was read
	 * @return HIH_DATA_NACK or HIH_DATA_IntfErr if error (wrong value or sensor not present)
	 */
	uint8_t ReadData(void);

	/** @brief Converts rawH to 10*humidity[%RH]
	 *
	 * rawH interval is [0 ... (2^14 - 2)] for [0 ... 100] %RH
	 * output interval is [0 ... 1000]
	 */
	uint16_t GetHumidity(void);

	/** @brief Converts rawT to 10*temperature[degK]
	 *
	 * rawT interval is [0 ... (2^14 - 2)] for [-40 ... +125] degC
	 * output interval is [2331 ... 3981]. To convert in 10*degC substract 2731
	 */
	uint16_t GetTemperature(void);
};

} /* namespace */
#endif /* ext_HIHSENSOR_H_ */
