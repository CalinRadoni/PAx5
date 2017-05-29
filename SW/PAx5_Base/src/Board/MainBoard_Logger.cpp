/**
This file is part of PAx5 (https://github.com/CalinRadoni/PAx5)
Copyright (C) 2016, 2017 by Calin Radoni

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
	for(uint8_t i = 0; i < MaxLogSize; i++){
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
	if(ListIdx >= MaxLogSize)
		ListIdx = 0;
}

// -----------------------------------------------------------------------------
} /* namespace PAx5 */
