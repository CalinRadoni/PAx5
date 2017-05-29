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

#include <NetWorker.h>
#include "version.h"
#include "Node_Config.h"
#include "MainBoard.h"
#include "dev_LED.h"
#include "cpu_Entropy.h"
#include "dev_RFM69.h"
#include "cpu_Text.h"

#include "cpu_I2C.h"
#include "ext_HIHSensor.h"

// -----------------------------------------------------------------------------

PAx5Node::Node_Config nodeConfiguration;
PAx5CommProtocol::NetWorker nodeProtocol;
PAx5CommProtocol::NetWorker::RXCheck chkRes;
PAx5CommProtocol::TimedSlot *nodeTS;

// -----------------------------------------------------------------------------

int main(void)
{
	PAx5::BoardDefinion boardDefinition;
	boardDefinition.SetByType(PAx5::BoardDefinion::BoardType::PAx5_BaseBoard);

	if(PAx5::board.InitializeBoard(boardDefinition) != PAx5::MainBoard::Error::OK){
		PAx5::board.BlinkError();
		while(1){ /* infinite loop */ }
	}

	uint32_t timeStart, timeDelta, timeQ;
	uint8_t i;

	PAx5::EXT_HIHSensor hihSensor;
	uint16_t sH, sT;

	timeStart = PAx5::sysTickCnt;

	PAx5::sLED.ModeBlink(300, 300, 3);

	PAx5::entropy.CollectStart();

	PAx5::sTextOutput.InitBuffer();

	nodeConfiguration.ReadConfigInfo();
	if((nodeConfiguration.configInfo.verHi == 0) && (nodeConfiguration.configInfo.verLo == 0)) {
		// no configuration set in EEPROM, create a default one
		nodeProtocol.commProtocol.SetDefaultKey(&nodeProtocol.netCtx);
		nodeConfiguration.WriteKey((uint32_t*)&nodeProtocol.netCtx.key);
		nodeConfiguration.configInfo.address = PAx5CommProtocol::AddressClass::Address_NONE;
		nodeConfiguration.WriteFrequency((uint32_t)864000000);

		PAx5::sTextOutput.FormatAndOutputString("Default configuration created.\r\n");
	}
	if((nodeConfiguration.configInfo.verHi != SW_VER_MAJOR) || (nodeConfiguration.configInfo.verLo != SW_VER_MINOR)) {
		// current configuration in EEPROM must be updated
		nodeConfiguration.configInfo.verHi = SW_VER_MAJOR;
		nodeConfiguration.configInfo.verLo = SW_VER_MINOR;
		nodeConfiguration.WriteConfigInfo();
	}


	nodeConfiguration.configInfo.address = 7;


	nodeProtocol.Initialize(nodeConfiguration.configInfo.address, nodeConfiguration.ReadFrequency());
	nodeConfiguration.ReadKey((uint32_t*)&nodeProtocol.netCtx.key);
	PAx5::sRadio.SetMode(PAx5::RM_RX);

	PAx5::sTextOutput.FormatAndOutputString("PAx5 Node version %d.%d\r\n", nodeConfiguration.configInfo.verHi, nodeConfiguration.configInfo.verLo);
	PAx5::sTextOutput.FormatAndOutputString("Address: %d\r\n", nodeConfiguration.configInfo.address);
	PAx5::sTextOutput.FormatAndOutputString("Frequency: %d Hz\r\n", nodeConfiguration.ReadFrequency());
	PAx5::sTextOutput.FormatAndOutputString("Key: ");
	for(i=0; i<4; i++)
		PAx5::sTextOutput.FormatAndOutputString(" %8x", nodeProtocol.netCtx.key[i]);
	PAx5::sTextOutput.FormatAndOutputString("\r\n");
	PAx5::sTextOutput.Flush();

	timeQ = 0;

	while(1){
		// handle radio communication
		if(PAx5::sRadio.packetReceived){
			PAx5::sRadio.packetReceived = false;
			chkRes = nodeProtocol.CheckReceivedPacket();
			PAx5::sTextOutput.FormatAndOutputString("rx: %d\r\n", (uint8_t)chkRes); PAx5::sTextOutput.Flush();
			switch(chkRes){
			case PAx5CommProtocol::NetWorker::RXCheck::PacketOK:
				/* Received a data packet
				 * The communication slot is netWorker.dataPeer
				 * The data is in netWorker.commProtocol.packetRX[CP_PACKET_HDR_Length_POS]
				 */
				break;

			case PAx5CommProtocol::NetWorker::RXCheck::ReqAckReceived:
				/* REQ+ACK received, start sending data */
				nodeTS = nodeProtocol.dataPeer;

				PAx5::sTextOutput.FormatAndOutputString("REQ+ACK received\r\n", (uint8_t)chkRes); PAx5::sTextOutput.Flush();

				if(hihSensor.ReadData() == PAx5::EXT_HIHSensor::Status::DATA_OK){
					sH = hihSensor.GetHumidity();
					sT = hihSensor.GetTemperature();
					PAx5::sTextOutput.FormatAndOutputString("Data: %d %%RH %d degC", sH/10, (sT-2731)/10); PAx5::sTextOutput.Flush();
				}
				else{ sH = sT = 0xEEEE; }

				nodeProtocol.commProtocol.packetTX[PAx5CommProtocol::CP_PACKET_HEADER_LEN]     = (uint8_t)(sH >> 8);
				nodeProtocol.commProtocol.packetTX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 1] = (uint8_t)(sH & 0xFF);
				nodeProtocol.commProtocol.packetTX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 2] = (uint8_t)(sT >> 8);
				nodeProtocol.commProtocol.packetTX[PAx5CommProtocol::CP_PACKET_HEADER_LEN + 3] = (uint8_t)(sT & 0xFF);
				nodeProtocol.commProtocol.CreateDataPacket(nodeTS, 4, true);

				if(nodeProtocol.SendPacket()){
					nodeTS->UpdateTimeSlotAckWait(CP_TIME_WAIT_ACK);
					PAx5::sTextOutput.FormatAndOutputString("Data sent\r\n");
				}
				else {
					nodeTS->RestartTimeSlot(0);
					PAx5::sTextOutput.FormatAndOutputString("Failed to send data, RSSI = %d !\r\n", PAx5::sRadio.packetReceivedRSSI);
				}
				PAx5::sTextOutput.Flush();
				break;

			case PAx5CommProtocol::NetWorker::RXCheck::AckReceived:
				/* ACK received, continue sending data or close the timeslot if done */
				nodeTS = nodeProtocol.dataPeer;

				PAx5::sTextOutput.FormatAndOutputString("ACK received\r\n", (uint8_t)chkRes); PAx5::sTextOutput.Flush();

				nodeTS->RestartTimeSlot(0);
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
		if(timeStart != PAx5::sysTickCnt){
			timeDelta = PAx5::sysTickCnt - timeStart;
			timeStart = PAx5::sysTickCnt;

			PAx5::sLED.Pulse(timeDelta);
			nodeProtocol.DecreaseTimeSlots(timeDelta);

			if(timeDelta < timeQ) timeQ -= timeDelta;
			else{
				nodeTS = nodeProtocol.GetFreeSlot(PAx5CommProtocol::AddressClass::Address_Gateway);
				if(nodeTS != NULL){
					timeQ = 3000;

					PAx5::sLED.ModeOn(200);

					hihSensor.ReadInit();

					nodeTS->peerAddress = PAx5CommProtocol::AddressClass::Address_Gateway;
					nodeProtocol.commProtocol.CreateRequestPacket(nodeTS, &nodeProtocol.netCtx);
					if(nodeProtocol.SendPacket()) PAx5::sTextOutput.FormatAndOutputString("RSSI = %d, REQ sent\r\n", PAx5::sRadio.packetReceivedRSSI);
					else                       PAx5::sTextOutput.FormatAndOutputString("Failed to send REQ, RSSI = %d !\r\n", PAx5::sRadio.packetReceivedRSSI);
					PAx5::sTextOutput.Flush();

					PAx5::sRadio.SetMode(PAx5::RM_RX);
				}
			}
		}
	}
}
