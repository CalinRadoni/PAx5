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

#ifndef ENC_ChaCha20_H_
#define ENC_ChaCha20_H_

#include <stddef.h>
#include <stdint.h>

#include "CryptoDefs.h"

namespace PAx5CommProtocol {

class Enc_ChaCha20 {
protected:
	uint32_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15;

	void BlockNoSum(void);
	void Block(void);

	/**
	 * \brief Initialize the state based on CryptoContext.
	 * \details This function is called from the 'EncryptBlock' function.
	 */
	void InitState(CryptoContext *ctx);

public:
	Enc_ChaCha20();

	/**
	 * \brief Encrypt a block of data.
	 *
	 * \warning Maximum usable length is 64 bytes (512 bits).
	 * \warning No operation is performed if ctx==NULL !
	 */
	void EncryptBlock(CryptoContext *ctx, uint8_t*, uint8_t);
};

} /* namespace */

#endif /* ENC_ChaCha20_H_ */
