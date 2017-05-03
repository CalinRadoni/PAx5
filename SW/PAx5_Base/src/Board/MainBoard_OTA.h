/**
 * created 2016.11.05 by Calin Radoni
 */

#ifndef SRC_BOARD_MAINBOARD_OTA_H_
#define SRC_BOARD_MAINBOARD_OTA_H_

#include "MainBoard_Base.h"

namespace PAx5 {

// TODO ExtFlash-OTA: Rebuild this class. ExtFlash must have high level functions not this class !

/**
 * \brief High level OTA
 *
 * \details This class takes care of writing the new firmware in the external FLASH.
 *          After the firmware is written, to transfer it in internal FLASH, call TransferFirmware function.
 *
 * \code{.cpp}
 * sMainBoardOTA.InitDataAndExtFLASH();
 * while(!all_data_added){
 *     if(!sMainBoardOTA.AddData(..., ...))
 *         return false;
 * }
 * if(!sMainBoardOTA.Finalize()) return false;
 * if(sMainBoardOTA.ComputeCRCforStoredFirmware() != correctCRC) return false;
 * if(!sMainBoardOTA.TransferFirmware()) return false;
 * return true;
 * \endcode
 */
class MainBoardOTA{
public:
	MainBoardOTA();

	void InitDataAndExtFLASH(void);
	bool AddData(uint8_t*, uint8_t);
	bool Finalize(void);

	uint32_t ComputeCRCforStoredFirmware(void);

	/**
	 * \brief Transfer downloaded firmware form external FLASH to internal FLASH
	 *
	 * \details The function will return false if:
	 *   - firmwareSize is 0
	 *   - firmwareSize > internalFlashSize
	 *   - failed to initialize external FLASH
	 *   - failed to reconfigure SPI1
	 *   - failed to unlock internal FLASH
	 *
	 * Onboard LED is turned ON after previous checks
	 * When writing is finalized:
	 *   - LED blinks 20 * (short-long) if writing failed or 5 short pulses if OK
	 *   - the board is reset
	 *
	 * \attention This function will not return if writing started, the board will reset.
	 */
	bool TransferFirmware(void);

private:
	uint32_t currentFlashAddr;
	uint16_t flashBuffIdx;
	uint32_t firmwareSize;

	void InternalInit(void);
};

} /* namespace */

#endif /* SRC_BOARD_MAINBOARD_OTA_H_ */
