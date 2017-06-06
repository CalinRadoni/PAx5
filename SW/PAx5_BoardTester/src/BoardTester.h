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
	void TestI2C_TSL2561(void);
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
