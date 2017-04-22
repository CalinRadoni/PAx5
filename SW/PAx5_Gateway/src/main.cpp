/**
 * created 2016.11.26 by Calin Radoni
 */

#include "version.h"
#include "GW_Config.h"
#include "MainBoard.h"
#include "dev_LED.h"
#include "cpu_Entropy.h"
#include "dev_RFM69.h"
#include "PAW_NetWorker.h"

#include "cpu_Text.h"

// -----------------------------------------------------------------------------

PAx5Gateway::GW_Config gatewayConfiguration;
PAx5CommProtocol::PAW_NetWorker gateProtocol;
PAx5CommProtocol::PAW_NetWorker::PAW_RXCheckResult chkRes;
PAx5CommProtocol::PAW_TimedSlot *nodeTS;

// -----------------------------------------------------------------------------

int main(void)
{
	uint32_t boardCapabilities;
	uint32_t timeS, timeDelta;
	uint8_t i, rxDataLen;

	boardCapabilities = PAx5_BOARD_HW_USART | PAx5_BOARD_HW_I2C | PAx5_BOARD_HW_RFM69HW | PAx5_BOARD_HW_ExtFLASH;
	PAx5::board.InitializeBoard(boardCapabilities);

	timeS = PAx5::sysTickCnt;

	PAx5::sLED.ModeBlink(300, 300, 3);

	PAx5::entropy.CollectStart();

	PAx5::sTextOutput.InitBuffer();

	gatewayConfiguration.ReadConfigInfo();
	if((gatewayConfiguration.configInfo.verHi == 0) && (gatewayConfiguration.configInfo.verLo == 0)) {
		// no configuration set in EEPROM, create a default one
		gateProtocol.commProtocol.SetDefaultKey(&gateProtocol.netCtx);
		gatewayConfiguration.WriteKey((uint32_t*)&gateProtocol.netCtx.key);
		gatewayConfiguration.configInfo.address = PAx5CommProtocol::AddressClass::Address_Gateway;
		gatewayConfiguration.WriteFrequency((uint32_t)864000000);

		PAx5::sTextOutput.FormatAndOutputString("Default configuration created.\r\n");
	}
	if((gatewayConfiguration.configInfo.verHi != SW_VER_MAJOR) || (gatewayConfiguration.configInfo.verLo != SW_VER_MINOR)) {
		// current configuration in EEPROM must be updated
		gatewayConfiguration.configInfo.verHi = SW_VER_MAJOR;
		gatewayConfiguration.configInfo.verLo = SW_VER_MINOR;
		gatewayConfiguration.WriteConfigInfo();
	}

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
			case PAx5CommProtocol::PAW_NetWorker::chk_PacketOK:
				/* Received a data packet
				 * The communication slot is netWorker.dataPeer
				 * The data is in netWorker.commProtocol.packetRX[CP_PACKET_HEADER_LEN]
				 */

				rxDataLen = gateProtocol.commProtocol.packetRX[CP_PACKET_HDR_Length_POS] - CP_PACKET_OVERHEAD;
				PAx5::sTextOutput.FormatAndOutputString("Len = %d, Data =", rxDataLen);
				for(i=0; i<rxDataLen; i++)
					PAx5::sTextOutput.FormatAndOutputString(" %2x", gateProtocol.commProtocol.packetRX[CP_PACKET_HEADER_LEN + i]);
				PAx5::sTextOutput.FormatAndOutputString("\r\n");

				cval = gateProtocol.commProtocol.packetRX[CP_PACKET_HEADER_LEN];
				cval = cval << 8;
				cval += gateProtocol.commProtocol.packetRX[CP_PACKET_HEADER_LEN + 1];
				PAx5::sTextOutput.FormatAndOutputString("10*RH = %d\t\t", cval);

				cval = gateProtocol.commProtocol.packetRX[CP_PACKET_HEADER_LEN + 2];
				cval = cval << 8;
				cval += gateProtocol.commProtocol.packetRX[CP_PACKET_HEADER_LEN + 3];
				cval -= 2731;
				PAx5::sTextOutput.FormatAndOutputString("T = %d 10*degC", cval);

				PAx5::sTextOutput.FormatAndOutputString("\r\n");
				PAx5::sTextOutput.Flush();

				gateProtocol.dataPeer->RestartTimeSlot(0);
				break;

			case PAx5CommProtocol::PAW_NetWorker::chk_ReqAckReceived:
				/* REQ+ACK received, start sending data */
				break;

			case PAx5CommProtocol::PAW_NetWorker::chk_AckReceived:
				/* ACK received, continue sending data or close the timeslot if done */
				break;

			case PAx5CommProtocol::PAW_NetWorker::chk_AddrRequest:
				break;

			case PAx5CommProtocol::PAW_NetWorker::chk_Bcast:
				break;

			default:
				break;
			}

			PAx5::sRadio.SetMode(PAx5::RM_RX);
		}

		// decrease time in sLED and communication slots
		if(timeS != PAx5::sysTickCnt){
			timeDelta = PAx5::sysTickCnt - timeS;
			timeS = PAx5::sysTickCnt;

			PAx5::sLED.Pulse(timeDelta);
			gateProtocol.DecreaseTimeSlots(timeDelta);
		}
	}

}
