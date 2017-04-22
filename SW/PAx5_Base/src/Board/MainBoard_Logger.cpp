/**
 * created 2016.10.30 by Calin Radoni
 */

#include "MainBoard_Logger.h"

// -----------------------------------------------------------------------------

void assertionFailed(const uint8_t FileID, uint16_t assertLine)
{
	PAx5::hwLogger.AddEntry(FileID, 0, assertLine);
}

// -----------------------------------------------------------------------------

namespace PAx5 {

// -----------------------------------------------------------------------------

MainBoardLogger hwLogger;

extern volatile uint32_t sysTickCnt;

// -----------------------------------------------------------------------------

MainBoardLogger::MainBoardLogger() {
	Initialize();
}

// -----------------------------------------------------------------------------

void MainBoardLogger::Initialize(void) {
	for(uint8_t i = 0; i < MAX_LOG_SIZE; i++){
		List[i].time   = 0;
		List[i].source = 0;
		List[i].code   = 0;
		List[i].data   = 0;
	}
	ListIdx = 0;
}

void MainBoardLogger::AddEntry(uint8_t source, uint8_t code, uint16_t data) {
	List[ListIdx].time   = sysTickCnt;
	List[ListIdx].source = source;
	List[ListIdx].code   = code;
	List[ListIdx].data   = data;
	ListIdx++;
	if(ListIdx >= MAX_LOG_SIZE)
		ListIdx = 0;
}

// -----------------------------------------------------------------------------
} /* namespace PAx5 */
