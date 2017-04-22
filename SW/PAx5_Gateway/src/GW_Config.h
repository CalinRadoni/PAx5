/**
 * created 2016.11.27 by Calin Radoni
 */

#ifndef GW_CONFIG_H_
#define GW_CONFIG_H_

#include "defines.h"

namespace PAx5Gateway {

// -----------------------------------------------------------------------------

/** EEPROM Configuration
 *
 *  0: version major | version minor | node address | reserved
 *
 *  1: network key[0]
 *  2: network key[1]
 *  3: network key[2]
 *  4: network key[3]
 *  5: reserved
 *  6: reserved
 *  7: reserved
 *  8: reserved
 *
 *  9: frequency
 * 10: (int8_t)power | unused | unused | unused
 */

// -----------------------------------------------------------------------------

struct BoardConfigInfo
{
	uint8_t verHi;
	uint8_t verLo;
	uint8_t address;
	uint8_t reserved;
};

class GW_Config
{
public:
	GW_Config();

	BoardConfigInfo configInfo;
	void ReadConfigInfo(void);
	bool WriteConfigInfo(void);

	void ReadKey(uint32_t*);
	bool WriteKey(uint32_t*);

	uint32_t ReadFrequency(void);
	bool WriteFrequency(uint32_t);
};

} /* namespace */

#endif
