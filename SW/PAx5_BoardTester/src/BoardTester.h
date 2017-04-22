/**
 * created 2016.07.25 by Calin Radoni
 */

#ifndef BoardTester_H_
#define BoardTester_H_

#include "MainBoard.h"

namespace PAx5 {

#define TestBuffLen 128

class BoardTester {
protected:

	uint8_t buffS[TestBuffLen];
	uint8_t buffD[TestBuffLen];

public:
	BoardTester();

	void InteractiveTest(void);

	void TestBoard(void);
	void TestEntropy(void);
	void TestEntropyADC(void);
	void TestADC(void);
	void TestI2C(void);
	void TestI2C_HIH(void);
	void TestI2CSlave(void);
	void TestWS2812(void);
	void TestEcryption(void);
	void TestFLASHProg(void);
	void TestEEPROMProg(void);
	void TestExtFlash(void);
	void TestCRC(void);
	void TestPAWPackets(void);
	void TestCopy(void);

	void ShowHWLog(void);
	void ClearHWLog(void);
};

}

#endif /* BoardTester_H_ */
