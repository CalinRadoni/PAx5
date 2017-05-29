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

#ifndef CMAC_ChaCha20_H_
#define CMAC_ChaCha20_H_

#include <stddef.h>
#include <stdint.h>

#include "CryptoDefs.h"
#include "Enc_ChaCha20.h"

namespace PAx5CommProtocol {

class CMAC_ChaCha20 {
protected:
	void LeftShift(uint8_t*);

	uint8_t k1[CMAC_DS], k2[CMAC_DS];

	/**
	 * \brief Initialize the keys for signing
	 *
	 * \details This function generates the CMAC/OMAC keys.
	 *          It is called from the Sign function.
	 */
	void InitKeys(CryptoContext *ctx);

public:
	CMAC_ChaCha20();

	Enc_ChaCha20  *chacha;

	/**
	 * \brief Sign some data
	 *
	 * \details This function signs the data buffer passed as parameter using the ChaCha20 algorithm.
	 *
	 * \warning Do not forget to set ctx->cnonce for signing before calling this function.
	 *
	 * \param packet    Data to be signed
	 * \param packetLen Length of data to be signed
	 * \param ctx       The crypto context
	 * \return Return value is in cmac[CMAC_DS].
	 */
	void Sign(CryptoContext *ctx, uint8_t* packet, uint8_t packetLen);

	uint8_t cmac[CMAC_DS];
};

} /* namespace */

#endif /* CMAC_ChaCha20_H_ */
