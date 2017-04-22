/**
 * created 2016.07.06 by Calin Radoni
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

	/** Initialize the state based on CryptoContext.
	 *
	 * This function is called from the 'EncryptBlock' function.
	 */
	void InitState(CryptoContext *ctx);

public:
	Enc_ChaCha20();

	/** Encrypt a block of data.
	 *
	 * @warning Maximum usable length is 64 bytes (512 bits).
	 *
	 * @warning No operation is performed if ctx==NULL !
	 */
	void EncryptBlock(CryptoContext *ctx, uint8_t*, uint8_t);
};

} /* namespace */

#endif /* ENC_ChaCha20_H_ */
