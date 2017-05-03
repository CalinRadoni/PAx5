/**
 * created 2016.10.30 by Calin Radoni
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
