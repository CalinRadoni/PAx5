/**
 * created 2016.08.07 by Calin Radoni
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

	/** Initialize the keys for signing
	 *
	 * This function generates the CMAC/OMAC keys.
	 * It is called from the Sign function.
	 */
	void InitKeys(CryptoContext *ctx);

public:
	CMAC_ChaCha20();

	Enc_ChaCha20  *chacha;

	/** Sign some data
	 *
	 * This function signs the data buffer passed as parameter using the ChaCha20 algorithm.
	 *
	 * @warning Do not forget to set ctx->cnonce for signing before calling this function.
	 *
	 * @param packet Data to be signed
	 * @param packetLen Length of data to be signed
	 * @param ctx The crypto context
	 * @return Return value is in cmac[CMAC_DS].
	 */
	void Sign(CryptoContext *ctx, uint8_t* packet, uint8_t packetLen);

	uint8_t cmac[CMAC_DS];
};

} /* namespace */

#endif /* CMAC_ChaCha20_H_ */
