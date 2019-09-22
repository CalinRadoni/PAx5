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

#include "version.h"
#include "GW_Config.h"
#include "NetWorker.h"

#include "MainBoard.h"
#include "cpu_Entropy.h"
#include "cpu_Text.h"

#include "dev_LED.h"
#include "dev_RFM69.h"

// -----------------------------------------------------------------------------

PAx5Gateway::GW_Config gatewayConfiguration;
PAx5CommProtocol::NetWorker gateProtocol;
PAx5CommProtocol::NetWorker::RXCheck chkRes;
PAx5CommProtocol::TimedSlot *nodeTS;

// -----------------------------------------------------------------------------

int main(void)
{
	// Initialize the board
	PAx5::BoardDefinion boardDefinition;
	boardDefinition.SetByType(PAx5::BoardDefinion::BoardType::PAx5_BaseBoard);
	if (PAx5::board.InitializeBoard(boardDefinition) != PAx5::MainBoard::Error::OK) {
		PAx5::board.BlinkError();
		while (1) {
			/* infinite loop */
		}
	}

	uint8_t i, rxDataLen;

	uint32_t timeS = PAx5::sysTickCnt;

	PAx5::sLED.ModeBlink(300, 300, 3);

	PAx5::entropy.CollectStart();

	PAx5::sTextOutput.InitBuffer();

	// read current configuration from gateway
	gatewayConfiguration.ReadConfigInfo();
	if ((gatewayConfiguration.configInfo.verHi == 0) && (gatewayConfiguration.configInfo.verLo == 0)) {
		// no configuration set in EEPROM, create a default one
		gateProtocol.commProtocol.SetDefaultKey(&gateProtocol.netCtx);
		gatewayConfiguration.WriteKey((uint32_t*)&gateProtocol.netCtx.key);
		gatewayConfiguration.configInfo.address = PAx5CommProtocol::AddressClass::Address_Gateway;
		gatewayConfiguration.WriteFrequency((uint32_t)864000000);

		PAx5::sTextOutput.FormatAndOutputString("Default configuration created.\r\n");
	}
	if ((gatewayConfiguration.configInfo.verHi != SW_VER_MAJOR) || (gatewayConfiguration.configInfo.verLo != SW_VER_MINOR)) {
		// current configuration in EEPROM must be updated
		gatewayConfiguration.configInfo.verHi = SW_VER_MAJOR;
		gatewayConfiguration.configInfo.verLo = SW_VER_MINOR;
		gatewayConfiguration.WriteConfigInfo();
	}

	// I am gateway so my address, at least the public one, should be 1
	gatewayConfiguration.configInfo.address = 1;

	gateProtocol.Initialize(gatewayConfiguration.configInfo.address, gatewayConfiguration.ReadFrequency());
	gatewayConfiguration.ReadKey((uint32_t*)&gateProtocol.netCtx.key);
	PAx5::sRadio.SetMode(PAx5::RM_RX);


	PAx5::sTextOutput.FormatAndOutputString("PAx5 Gateway version %d.%d\r\n", gatewayConfiguration.configInfo.verHi, gatewayConfiguration.configInfo.verLo);
	PAx5::sTextOutput.FormatAndOutputString("Address: %d\r\n", gatewayConfiguration.configInfo.address);
	PAx5::sTextOutput.FormatAndOutputString("Frequency: %d Hz\r\n", gatewayConfiguration.ReadFrequency());
	PAx5::sTextOutput.FormatAndOutputString("Key: ");
	for(i=0; i<4; i++)
		PAx5::sTextOutput.FormatAndOutputString(" %8x", gateProtocol.netCtx.key[i]);
	PAx5::sTextOutput.FormatAndOutputString("\r\n");
	PAx5::sTextOutput.Flush();

	uint16_t cval;

	while(1){
		// handle radio communication
		if(PAx5::sRadio.packetReceived){
			PAx5::sRadio.packetReceived = false;
			chkRes = gateProtocol.CheckReceivedPacket();
			PAx5::sTextOutput.FormatAndOutputString("rx: %d\r\n", (uint8_t)chkRes); PAx5::sTextOutput.Flush();
			switch(chkRes){
			  case PAx5CommProtocol::NetWorker::RXCheck::PacketOK:
				/* Received a data packet
				 * The communication slot is netWorker.dataPeer
				 * The data is in netWorker.commProtocol.packetRX[CP_PACKET_HEADER_LEN]
				 */

				rxDataLen = gateProtocol.commProtocol.packetRX[CP_PACKET_HDR_Length_POS] - PAx5CommProtocol::CP_PACKET_OVERHEAD;
				PAx5::sTextOutput.FormatAndOutputString("Len = %d, Data =", rxDataLen);
				for(i=0; i<rxDataLen; i++)
					PAx5::sTextOutput.FormatAndOutputString(" %2x", gateProtocol.commProtocol.packetRX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + i]);
				PAx5::sTextOutput.FormatAndOutputString("\r\n");

				cval = gateProtocol.commProtocol.packetRX[PAx5CommProtocol::CP_PACKET_HEADER_LEN];
				cval = cval << 8;
				cval += gateProtocol.commProtocol.packetRX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 1];
				PAx5::sTextOutput.FormatAndOutputString("10*RH = %d\t\t", cval);

				cval = gateProtocol.commProtocol.packetRX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 2];
				cval = cval << 8;
				cval += gateProtocol.commProtocol.packetRX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 3];
				cval -= 2731;
				PAx5::sTextOutput.FormatAndOutputString("T = %d 10*degC", cval);

				PAx5::sTextOutput.FormatAndOutputString("\r\n");
				PAx5::sTextOutput.Flush();

				gateProtocol.dataPeer->RestartTimeSlot(0);
				break;

			  case PAx5CommProtocol::NetWorker::RXCheck::ReqAckReceived:
				/* REQ+ACK received, start sending data */
				break;

			  case PAx5CommProtocol::NetWorker::RXCheck::AckReceived:
				/* ACK received, continue sending data or close the timeslot if done */
				break;

			  case PAx5CommProtocol::NetWorker::RXCheck::AddrRequest:
				break;

			  case PAx5CommProtocol::NetWorker::RXCheck::Bcast:
				break;

			  default:
				break;
			}

			PAx5::sRadio.SetMode(PAx5::RM_RX);
		}

		// decrease time in sLED and communication slots
		if(timeS != PAx5::sysTickCnt){
			uint32_t timeDelta = PAx5::sysTickCnt - timeS;
			timeS = PAx5::sysTickCnt;

			PAx5::sLED.Pulse(timeDelta);
			gateProtocol.DecreaseTimeSlots(timeDelta);
		}
	}

}
