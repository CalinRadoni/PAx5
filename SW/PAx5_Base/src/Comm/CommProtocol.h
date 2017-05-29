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

#ifndef COMMPROTOCOL_H_
#define COMMPROTOCOL_H_

#include <CommDefines.h>
#include <Enc_ChaCha20.h>
#include <CMAC_ChaCha20.h>

#include <TimedSlot.h>

namespace PAx5CommProtocol {

const     uint8_t CP_PACKET_RAW_OFFSET   = 2;

const     uint8_t CP_PACKET_HEADER_LEN   = 16;
const     uint8_t CP_PACKET_HASH_LEN     = 8;
constexpr uint8_t CP_PACKET_OVERHEAD     = (CP_PACKET_HEADER_LEN + CP_PACKET_HASH_LEN);
const     uint8_t CP_PACKET_MAX_LEN      = 64;
constexpr uint8_t CP_PACKET_MAX_DATA_LEN = (CP_PACKET_MAX_LEN - CP_PACKET_OVERHEAD);

#define CP_PACKET_HDR_Length_POS  0
#define CP_PACKET_HDR_DstAddr_POS 1
#define CP_PACKET_HDR_SrcAddr_POS 2
#define CP_PACKET_HDR_FLAGS_POS   3
#define CP_PACKET_HDR_ID_POS      4
#define CP_PACKET_HDR_NONCE0_POS  8
#define CP_PACKET_HDR_NONCE1_POS  12

#define CP_PACKET_HDR_FLAGS 0xF0
#define CP_PACKET_HDR_REQ   0x80
#define CP_PACKET_HDR_ACK   0x40
#define CP_PACKET_HDR_WAIT  0x20

#define CP_TIME_WAIT_REQACK  3000
#define CP_TIME_WAIT_DATA    1000
#define CP_TIME_WAIT_ACK     1000
#define CP_TIME_Process_Cmd  5000

/**
 * \brief Packet format version 1
 *
 * \details
 * Length and DstAddr are shared with RFM69 layer. MUST keep their positions.
 *
 * - Header         CP_PACKET_HEADER_LEN bytes
 *    - Length       1 byte = the length of entire packet
 *    - DstAddr      1 byte
 *    - SrcAddr      1 byte
 *    - Type         1 byte
 *       - Request       1 bit = TX Request packet
 *       - Acknowledge   1 bit = (Request == 1 ? TX Request acknowledge: RX acknowledge)
 *       - Wait          1 bit = server has data do send, do not enter sleep mode yet
 *       - unused        1 bits
 *       - Version       4 bits
 *    - SessionID     4 bytes
 *    - Nonce         8 bytes
 * - Encrypted Data max. 40 bytes
 * - AuthHash	      CP_PACKET_HASH_LEN bytes
 */

class CommProtocol {
public:
	CommProtocol();

	/** @brief Buffers for input and output data packets */
	uint8_t packetIn[CP_PACKET_MAX_LEN + CP_PACKET_RAW_OFFSET];
	uint8_t packetOut[CP_PACKET_MAX_LEN + CP_PACKET_RAW_OFFSET];
	uint8_t *packetRX;
	uint8_t *packetTX;

	void SetDefaultKey(CryptoContext*);

	/** @brief This node address. This is the address used as source for building packets. */
	uint8_t nodeAddress;

	void Initialize(uint8_t thisNodeAddress);

	/** @brief Create a session key for a PAW_TimedSlot
	 *
	 * Using client and server nonces it generates a session key in `serverSlot.sessionEncCtx`
	 * A nonce for communication is set in `serverSlot.sessionEncCtx`
	 */
	void CreateSessionKey(TimedSlot*);


	/** @brief Build the header in packetOut
	 *
	 * Set the packet's header according to defined packet format.
	 *
	 * This function will generate a new nonce and will fill ctx's nonce.
	 *
	 * Using this function will ease the changes of code if packet format will be changed.
	 */
	void BuildPacketHeader(CryptoContext* ctx, uint8_t dst, uint8_t flags, uint8_t dataLength, uint32_t sessionID);

	/** @brief Creates a request packet
	 *
	 * Requires: ts->peerAddress
	 *
	 * Generates a new session ID and store it in _ts_
	 * Initialize _ts_ with CP_TIME_WAIT_REQACK
	 */
	void CreateRequestPacket(TimedSlot* ts, CryptoContext* ctx);

	/** @brief Creates a request+acknowledge packet
	 *
	 * Requires: ts->peerAddress and ts~SetSessionID
	 *
	 * Update _ts_ to CP_TIME_WAIT_REQACK
	 */
	void CreateReqAckPacket(TimedSlot* ts, CryptoContext* ctx);

	/** @brief Creates an acknowledge packet
	 *
	 * Requires: ts->peerAddress and ts~SetSessionID
	 * Set the packet's payload before calling this function !
	 */
	void CreateAckPacket(TimedSlot* ts, uint8_t payloadLength);

	/** @brief Creates an acknowledge packet
	 *
	 * Requires: ts->peerAddress and ts~SetSessionID
	 * Set the packet's payload before calling this function !
	 */
	void CreateDataPacket(TimedSlot* ts, uint8_t payloadLength, bool lastPacket);

	/** \brief Results returned by CheckAddresses function */
	enum class AddrChk : uint8_t {
		wrongCombo = 0,
		srcNone_dstGW = 1,
		srcOK_dstGW = 2,
		srcOK_dstME = 3,
		srcOK_dstBcast = 4
	};
	/**
	 * \brief Check source and destination addresses in #packetIn
	 */
	AddrChk CheckAddresses(void);

	/** @brief Encrypt, sign and generate a new nonce
	 *
	 * This function encrypts packet's data and sign the whole packet.
	 * Before signing, the nonce will be changed by calling SetNonceCounterForSigning.
	 * After signing the hash will be inserted in the packet.
	 *
	 * @warning The packet must be filled before calling this function. The length must be
	 * set for entire packet, including hash.
	 */
	void EncryptAndSign(CryptoContext* ctx);

	/** @brief Check packet length, signature and decrypt
	 *
	 * This function checks the length stored in packet[0],
	 * compute signature and make a constant time comparison of the signatures.
	 * If signature is OK the packet will be decrypted.
	 */
	bool CheckSignatureAndDecrypt(CryptoContext* ctx);

	/** MSB BYTE* -> DWORD
	 *
	 * Converts 4 uint8_t, MSB first, to one uint32_t.
	 */
	uint32_t BP2DW(uint8_t*);

	/** MSB DWORD -> BYTE*
	 *
	 * Converts an uint32_t to 4 uint8_t, MSB first.
	 */
	void DW2BP(uint32_t, uint8_t*);

protected:
	Enc_ChaCha20 chacha;
	CMAC_ChaCha20 cmac;

	uint32_t currentSessionID;
	uint32_t currentNonce[2];

	/** @brief Compare two hashes
	 *
	 * Compares the hash from packet with a computed one (cmac.cmac).
	 * The comparison runs in constant time, if `return res == 0` runs in constant time :)
	 * to avoid timing attacks.
	 *
	 * @param [in] The index where the hash starts in packetRX
	 */
	bool CompareHashInConstantTime(uint8_t hashStartIdx);

	void GetNewSessionID(void);
	void NewNonce(void);
};

} /* namespace */

#endif
