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

#ifndef SRC_BOARD_MAINBOARD_LOGGER_H_
#define SRC_BOARD_MAINBOARD_LOGGER_H_

#include "defines.h"

// -----------------------------------------------------------------------------

#define FILEID(id) \
	const uint8_t FileID = id; \
    void AssertFileID##id(void) {}

#define ASSERT(expr) \
    if (expr) \
        {} \
    else \
        assertionFailed(FileID, __LINE__)

// -----------------------------------------------------------------------------

namespace PAx5 {

const uint8_t MaxLogSize = 10;

/** Log entry definition */
struct MainBoardLogEntry {
	uint32_t time;
	uint8_t  source;
	uint8_t  code;
	uint16_t data;
};

/**
 * \brief A simple logger
 *
 * \details This is a simple logger which uses RAM as storage.
 *          To be as fast as possible and low on memory usage/entry I have used an array.
 *          The time is set from system's time.
 */
class MainBoardLogger {
private:
	volatile uint8_t ListIdx;

public:
	MainBoardLogger();

	/** Initialize the list by setting all values to zero */
	void Initialize(void);

	/**
	 * \brief Add an entry to the end of the list
	 *
	 * \details If there are allready maxRecCount entries the old one will be deleted.
	 *          The value 0 for #code parameter is used for failed assertions !
	 *
	 * \param source The ID of the source of the message;
	 * \param code User code to identify the event;
	 * \param data User data for the event.
	 */
	void AddEntry(uint8_t source, uint8_t code, uint16_t data);

	MainBoardLogEntry List[MaxLogSize];
};

extern MainBoardLogger hwLogger;

} /* namespace PAx5 */

#endif /* SRC_BOARD_MAINBOARD_LOGGER_H_ */
