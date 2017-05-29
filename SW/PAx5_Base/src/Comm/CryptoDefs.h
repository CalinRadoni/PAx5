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

#ifndef CryptoDefs_H_
#define CryptoDefs_H_

namespace PAx5CommProtocol {

struct CryptoContext {
	bool use256bitsEncKey;

	/**
	 * \brief The Key
	 * \details This could accomodate a 256 bits key.
	 *          If using a 128 bits key, use the first dwords (0..3).
	 */
	uint32_t key[8];

	/**
	 * \brief Counter and nonce
	 *
	 * \details
	 * This is a 128 bits nonce but can be used as a combination between a nonce and a counter
	 * (Example: 96 bits nonce and 32 bits counter).
	 *
	 * Example 1: Standard usage
	 * First dword (cnonce[0]) can be used as counter and dwords 1..3 can be used as nonce.
	 * In this configuration the counter, cnonce[0], can be set to 0 for message signing
	 * and starting with 1 for message encryption.
	 *
	 * Using ChaCha20 the block length is 64 bytes.
	 * The counter should be 1 for the first 64 bytes (512 bits) block, 2 for the second block and so on.
	 *
	 * Example 2: Short messages
	 * For messages with maximum length <= block length, all 4 dwords can be used as nonce for encryption
	 * and a derived nonce can be used for signing.
	 * Derived nonce can be as simple as nonce++, nonce[0]-- or nonce[3]^0x01
	 *
	 * Example 3: Short messages and short nonce
	 * The nonce must be sent along the message. To shorten the complete data packet the nonce can be shorted.
	 * To use a 64 bits nonce set cnonce[2] = cnonce[0] and cnonce[3] = cnonce[1]
	 * and use it for encryption then derive a nonce for signing.
	 * Derived nonce can be as simple as nonce++, nonce[0]-- or nonce[3]^0x01
	 */
	uint32_t cnonce[4];
};

#define CMAC_BITS 128

#if CMAC_BITS == 128
	#define CMAC_DS 16
#elif CMAC_BITS == 256
	#define CMAC_DS 32
#else
	#error "Wrong value for CMAC_BITS !"
#endif

} /* namespace */

#endif /* CryptoDefs_H_ */
