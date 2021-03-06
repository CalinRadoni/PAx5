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

#ifndef dev_RFM69_Regs_H_
#define dev_RFM69_Regs_H_

#include "MainBoard_Base.h"

namespace PAx5 {

const uint8_t RFM69_RegFifo          = 0x00; // FIFO read/write access (reset: 0x00)
const uint8_t RFM69_RegOpMode        = 0x01; // Operating modes of the transceiver (reset: 0x04)
const uint8_t RFM69_RegDataModul     = 0x02; // Data operation mode and Modulation settings (reset: 0x00)
const uint8_t RFM69_RegBitrateMsb    = 0x03; // Bit Rate setting, Most Significant Bits (reset: 0x1A)
const uint8_t RFM69_RegBitrateLsb    = 0x04; // Bit Rate setting, Least Significant Bits (reset: 0x0B)
const uint8_t RFM69_RegFdevMsb       = 0x05; // Frequency Deviation setting, Most Significant Bits (reset: 0x00)
const uint8_t RFM69_RegFdevLsb       = 0x06; // Frequency Deviation setting, Least Significant Bits (reset: 0x52)
const uint8_t RFM69_RegFrfMsb        = 0x07; // RF Carrier Frequency, Most Significant Bits (reset: 0xE4)
const uint8_t RFM69_RegFrfMid        = 0x08; // RF Carrier Frequency, Intermediate Bits (reset: 0xC0)
const uint8_t RFM69_RegFrfLsb        = 0x09; // RF Carrier Frequency, Least Significant Bits (reset: 0x00)
const uint8_t RFM69_RegOsc1          = 0x0A; // RC Oscillators Settings (reset: 0x41)
const uint8_t RFM69_RegAfcCtrl       = 0x0B; // AFC control in low modulation index situations (reset: 0x00)
const uint8_t RFM69_Reserved0C       = 0x0C; // (reset: 0x02)
const uint8_t RFM69_RegListen1       = 0x0D; // Listen Mode settings (reset: 0x92)
const uint8_t RFM69_RegListen2       = 0x0E; // Listen Mode Idle duration (reset: 0xF5)
const uint8_t RFM69_RegListen3       = 0x0F; // Listen Mode Rx duration (reset: 0x20)
const uint8_t RFM69_RegVersion       = 0x10; // Module version (reset: 0x24)
const uint8_t RFM69_RegPaLevel       = 0x11; // PA selection and Output Power control (reset: 0x9F)
const uint8_t RFM69_RegPaRamp        = 0x12; // Control of the PA ramp time in FSK mode (reset: 0x09)
const uint8_t RFM69_RegOcp           = 0x13; // Over Current Protection control (reset: 0x1A)
const uint8_t RFM69_Reserved14       = 0x14; // (reset: 0x40)
const uint8_t RFM69_Reserved15       = 0x15; // (reset: 0xB0)
const uint8_t RFM69_Reserved16       = 0x16; // (reset: 0x7B)
const uint8_t RFM69_Reserved17       = 0x17; // (reset: 0x9B)
const uint8_t RFM69_RegLna           = 0x18; // LNA settings (reset: 0x08 recommended: 0x88)
const uint8_t RFM69_RegRxBw          = 0x19; // Channel Filter BW Control (reset: 0x86 recommended: 0x55)
const uint8_t RFM69_RegAfcBw         = 0x1A; // Channel Filter BW control during the AFC routine (reset: 0x8A recommended: 0x8B)
const uint8_t RFM69_RegOokPeak       = 0x1B; // OOK demodulator selection and control in peak mode (reset: 0x40)
const uint8_t RFM69_RegOokAvg        = 0x1C; // Average threshold control of the OOK demodulator (reset: 0x80)
const uint8_t RFM69_RegOokFix        = 0x1D; // Fixed threshold control of the OOK demodulator (reset: 0x06)
const uint8_t RFM69_RegAfcFei        = 0x1E; // AFC and FEI control and status (reset: 0x10)
const uint8_t RFM69_RegAfcMsb        = 0x1F; // MSB of the frequency correction of the AFC (reset: 0x00)
const uint8_t RFM69_RegAfcLsb        = 0x20; // LSB of the frequency correction of the AFC (reset: 0x00)
const uint8_t RFM69_RegFeiMsb        = 0x21; // MSB of the calculated frequency error (reset: 0x00)
const uint8_t RFM69_RegFeiLsb        = 0x22; // LSB of the calculated frequency error (reset: 0x00)
const uint8_t RFM69_RegRssiConfig    = 0x23; // RSSI-related settings (reset: 0x02)
const uint8_t RFM69_RegRssiValue     = 0x24; // RSSI value in dBm (reset: 0xFF)
const uint8_t RFM69_RegDioMapping1   = 0x25; // Mapping of pins DIO0 to DIO3 (reset: 0x00)
const uint8_t RFM69_RegDioMapping2   = 0x26; // Mapping of pins DIO4 and DIO5, ClkOut frequency (reset: 0x05 recommended: 0x07)
const uint8_t RFM69_RegIrqFlags1     = 0x27; // Status register: PLL Lock state, Timeout, RSSI > Threshold... (reset: 0x80)
const uint8_t RFM69_RegIrqFlags2     = 0x28; // Status register: FIFO handling flags... (reset: 0x00)
const uint8_t RFM69_RegRssiThresh    = 0x29; // RSSI Threshold control (reset: 0xFF recommended: 0xE4)
const uint8_t RFM69_RegRxTimeout1    = 0x2A; // Timeout duration between Rx request and RSSI detection (reset: 0x00)
const uint8_t RFM69_RegRxTimeout2    = 0x2B; // Timeout duration between RSSI detection and PayloadReady (reset: 0x00)
const uint8_t RFM69_RegPreambleMsb   = 0x2C; // Preamble length, MSB (reset: 0x00)
const uint8_t RFM69_RegPreambleLsb   = 0x2D; // Preamble length, LSB (reset: 0x03)
const uint8_t RFM69_RegSyncConfig    = 0x2E; // Sync Word Recognition control (reset: 0x98)
const uint8_t RFM69_RegSyncValue1    = 0x2F; // Sync Word byte 1 (reset: 0x00)
const uint8_t RFM69_RegSyncValue2    = 0x30; // Sync Word byte 2 (reset: 0x00)
const uint8_t RFM69_RegSyncValue3    = 0x31; // Sync Word byte 3 (reset: 0x00)
const uint8_t RFM69_RegSyncValue4    = 0x32; // Sync Word byte 4 (reset: 0x00)
const uint8_t RFM69_RegSyncValue5    = 0x33; // Sync Word byte 5 (reset: 0x00)
const uint8_t RFM69_RegSyncValue6    = 0x34; // Sync Word byte 6 (reset: 0x00)
const uint8_t RFM69_RegSyncValue7    = 0x35; // Sync Word byte 7 (reset: 0x00)
const uint8_t RFM69_RegSyncValue8    = 0x36; // Sync Word byte 8 (reset: 0x00)
const uint8_t RFM69_RegPacketConfig1 = 0x37; // Packet mode settings (reset: 0x10)
const uint8_t RFM69_RegPayloadLength = 0x38; // Payload length setting (reset: 0x40)
const uint8_t RFM69_RegNodeAdrs      = 0x39; // Node address (reset: 0x00)
const uint8_t RFM69_RegBroadcastAdrs = 0x3A; // Broadcast address (reset: 0x00)
const uint8_t RFM69_RegAutoModes     = 0x3B; // Auto modes settings (reset: 0x00)
const uint8_t RFM69_RegFifoThresh    = 0x3C; // Fifo threshold, Tx start condition (reset: 0x0F recommended: 0x8F)
const uint8_t RFM69_RegPacketConfig2 = 0x3D; // Packet mode settings (reset: 0x02)
const uint8_t RFM69_RegAesKey1       = 0x3E; // Cipher key byte 1 (reset: 0x00)
const uint8_t RFM69_RegAesKey2       = 0x3F; // Cipher key byte 2 (reset: 0x00)
const uint8_t RFM69_RegAesKey3       = 0x40; // Cipher key byte 3 (reset: 0x00)
const uint8_t RFM69_RegAesKey4       = 0x41; // Cipher key byte 4 (reset: 0x00)
const uint8_t RFM69_RegAesKey5       = 0x42; // Cipher key byte 5 (reset: 0x00)
const uint8_t RFM69_RegAesKey6       = 0x43; // Cipher key byte 6 (reset: 0x00)
const uint8_t RFM69_RegAesKey7       = 0x44; // Cipher key byte 7 (reset: 0x00)
const uint8_t RFM69_RegAesKey8       = 0x45; // Cipher key byte 8 (reset: 0x00)
const uint8_t RFM69_RegAesKey9       = 0x46; // Cipher key byte 9 (reset: 0x00)
const uint8_t RFM69_RegAesKey10      = 0x47; // Cipher key byte 10 (reset: 0x00)
const uint8_t RFM69_RegAesKey11      = 0x48; // Cipher key byte 11 (reset: 0x00)
const uint8_t RFM69_RegAesKey12      = 0x49; // Cipher key byte 12 (reset: 0x00)
const uint8_t RFM69_RegAesKey13      = 0x4A; // Cipher key byte 13 (reset: 0x00)
const uint8_t RFM69_RegAesKey14      = 0x4B; // Cipher key byte 14 (reset: 0x00)
const uint8_t RFM69_RegAesKey15      = 0x4C; // Cipher key byte 15 (reset: 0x00)
const uint8_t RFM69_RegAesKey16      = 0x4D; // Cipher key byte 16 (reset: 0x00)
const uint8_t RFM69_RegTemp1         = 0x4E; // Temperature Sensor control (reset: 0x01)
const uint8_t RFM69_RegTemp2         = 0x4F; // Temperature readout (reset: 0x00)
const uint8_t RFM69_RegTestLna       = 0x58; // Sensitivity boost (reset: 0x1B)
const uint8_t RFM69_RegTestPa1       = 0x5A; // High Power PA settings (reset: 0x55)
const uint8_t RFM69_RegTestPa2       = 0x5C; // High Power PA settings (reset: 0x70)
const uint8_t RFM69_RegTestDagc      = 0x6F; // Fading Margin Improvement (reset: 0x00 recommended: 0x30)
const uint8_t RFM69_RegTestAfc       = 0x71; // AFC offset for low modulation index AFC (reset: 0x00)

} /* namespace */

#endif
