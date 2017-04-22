/**
 * created 2016.07.06 by Calin Radoni
 */

#include "Enc_ChaCha20.h"

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

// NOTE Cortex-M0+ have only the RORS instruction but gcc is smart enough to transfor ROL into ROR :)
#define ROL(mX, mN) (mX = (mX<<mN) | (mX>>(32 - mN)))

// -----------------------------------------------------------------------------

// NOTE Cortex-M0+ have only the RORS instruction but gcc is smart enough to transfor ROL into ROR :)
#define QUARTER_ROUND(a, b, c, d) \
		a += b; d ^= a; ROL(d, 16); \
		c += d; b ^= c; ROL(b, 12); \
		a += b; d ^= a; ROL(d,  8); \
		c += d; b ^= c; ROL(b,  7);

// -----------------------------------------------------------------------------

Enc_ChaCha20::Enc_ChaCha20() { }

// -----------------------------------------------------------------------------

void Enc_ChaCha20::Block(void)
{
	uint32_t x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;

	x0  = s0;  x1  = s1;  x2  = s2;  x3  = s3;
	x4  = s4;  x5  = s5;  x6  = s6;  x7  = s7;
	x8  = s8;  x9  = s9;  x10 = s10; x11 = s11;
	x12 = s12; x13 = s13; x14 = s14; x15 = s15;

	for(uint8_t i = 0; i < 10; i++){
		QUARTER_ROUND( s0, s4, s8,  s12);
		QUARTER_ROUND( s1, s5, s9,  s13);
		QUARTER_ROUND( s2, s6, s10, s14);
		QUARTER_ROUND( s3, s7, s11, s15);
		QUARTER_ROUND( s0, s5, s10, s15);
		QUARTER_ROUND( s1, s6, s11, s12);
		QUARTER_ROUND( s2, s7, s8,  s13);
		QUARTER_ROUND( s3, s4, s9,  s14);
	}

	s0  += x0;  s1  += x1;  s2  += x2;  s3  += x3;
	s4  += x4;  s5  += x5;  s6  += x6;  s7  += x7;
	s8  += x8;  s9  += x9;  s10 += x10; s11 += x11;
	s12 += x12; s13 += x13; s14 += x14; s15 += x15;
}

// -----------------------------------------------------------------------------

void Enc_ChaCha20::InitState(CryptoContext *ctx)
{
	if(ctx==NULL) return;

	s0 = 0x61707865;
	if(ctx->use256bitsEncKey) { s1 = 0x3320646e; s2 = 0x79622d32; }
	else                      { s1 = 0x3120646e; s2 = 0x79622d36; }
	s3 = 0x6b206574;

	s4 = ctx->key[0]; s5 = ctx->key[1]; s6 = ctx->key[2]; s7 = ctx->key[3];
	if(ctx->use256bitsEncKey){ s8 = ctx->key[4]; s9 = ctx->key[5]; s10 = ctx->key[6]; s11 = ctx->key[7]; }
	else                     { s8 = ctx->key[0]; s9 = ctx->key[1]; s10 = ctx->key[2]; s11 = ctx->key[3]; }

	s12 = ctx->cnonce[0]; s13 = ctx->cnonce[1];
	s14 = ctx->cnonce[2]; s15 = ctx->cnonce[3];
}

// -----------------------------------------------------------------------------

void Enc_ChaCha20::EncryptBlock(CryptoContext *ctx, uint8_t* data, uint8_t length)
{
	uint8_t sb[4];
	uint8_t i, is, ix;
	uint8_t len;
	uint32_t sx;

	if(ctx==NULL) return;

	InitState(ctx);
	Block();

	len = length;
	if(len > 64)
		len = 64;

	for(i = 0; i < len; i++){
		is = i & 0x03;
		if(is == 0){
			ix = i >> 2;
			switch(ix){
				case 0  : sx = s0;  break;
				case 1  : sx = s1;  break;
				case 2  : sx = s2;  break;
				case 3  : sx = s3;  break;
				case 4  : sx = s4;  break;
				case 5  : sx = s5;  break;
				case 6  : sx = s6;  break;
				case 7  : sx = s7;  break;
				case 8  : sx = s8;  break;
				case 9  : sx = s9;  break;
				case 10 : sx = s10; break;
				case 11 : sx = s11; break;
				case 12 : sx = s12; break;
				case 13 : sx = s13; break;
				case 14 : sx = s14; break;
				case 15 : sx = s15; break;
			}
			sb[0] =  sx        & 0xFF;
			sb[1] = (sx >>  8) & 0xFF;
			sb[2] = (sx >> 16) & 0xFF;
			sb[3] = (sx >> 24) & 0xFF;
		}
		data[i] = data[i] ^ sb[is];
	}
}

// -----------------------------------------------------------------------------

} /* namespace */
