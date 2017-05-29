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

#include "CMAC_ChaCha20.h"

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

CMAC_ChaCha20::CMAC_ChaCha20()
{
	chacha = NULL;
}

// -----------------------------------------------------------------------------

void CMAC_ChaCha20::LeftShift(uint8_t* data)
{
	if(data==NULL) return;

	for(uint8_t i=0; i<(CMAC_DS-1); i++)
		data[i] = (data[i] << 1) | (data[i+1] >> 7);
	data[CMAC_DS-1] = (data[CMAC_DS-1] << 1);
}

// -----------------------------------------------------------------------------

/** Polynomials to derive keys for signing based on number of bits
 *
 *  64,4,3,1	-> 2^4  + 2^3 + 2^1 + 1 = 0x1B
 * 128,7,2,1	-> 2^7  + 2^2 + 2^1 + 1 = 0x87
 * 256,10,5,2	-> 2^10 + 2^5 + 2^2 + 1 = 0x425
 */
void CMAC_ChaCha20::InitKeys(CryptoContext *ctx)
{
	uint8_t i;

	if(ctx == NULL) return;
	if(chacha == NULL) return;

#if CMAC_BITS == 256
	for(i = 0; i < CMAC_DS; i++) k1[i] = 0;
	chacha->EncryptBlock(k1, CMAC_DS);
	i = (k1[0] >> 7) ? 1 : 0;
	LeftShift(k1);
	if(i != 0){
		k1[CMAC_DS - 2] = k1[CMAC_DS - 2] ^ 0x04;
		k1[CMAC_DS - 1] = k1[CMAC_DS - 1] ^ 0x25;
	}

	for(i = 0; i < CMAC_DS; i++) k2[i] = k1[i];
	i = (k1[0] >> 7) ? 1 : 0;
	LeftShift(k2);
	if(i != 0){
		k2[CMAC_DS - 2] = k2[CMAC_DS - 2] ^ 0x04;
		k2[CMAC_DS - 1] = k2[CMAC_DS - 1] ^ 0x25;
	}
#endif

#if CMAC_BITS == 128
	for(i = 0; i < CMAC_DS; i++) k1[i] = 0;
	chacha->EncryptBlock(ctx, k1, CMAC_DS);
	i = (k1[0] >> 7) ? 1 : 0;
	LeftShift(k1);
	if(i != 0)
		k1[CMAC_DS - 1] = k1[CMAC_DS - 1] ^ 0x87;

	for(i = 0; i < CMAC_DS; i++) k2[i] = k1[i];
	i = (k1[0] >> 7) ? 1 : 0;
	LeftShift(k2);
	if(i != 0)
		k2[CMAC_DS - 1] = k2[CMAC_DS - 1] ^ 0x87;
#endif
}

// -----------------------------------------------------------------------------

void CMAC_ChaCha20::Sign(CryptoContext *ctx, uint8_t* packet, uint8_t packetLen)
{
	uint8_t i, idx;
	uint8_t w[CMAC_DS];

	if(packet == NULL) return;
	if(ctx == NULL)    return;
	if(chacha == NULL) return;

	if(packetLen == 0){
		for(i=0; i<CMAC_DS; i++)
			cmac[i] = 0;
		return;
	}

	InitKeys(ctx);

	idx = 0;
	while(idx < packetLen){
		for(i=0; i<CMAC_DS; i++){
			if(idx < packetLen) w[i] = packet[idx];
			else{
				// padding
				if(idx == packetLen) w[i] = 0x80;
				else                 w[i] = 0x00;
			}
			idx++;
		}

		if(idx > CMAC_DS){
			// not the first block, xor with previous result
			for(i=0; i<CMAC_DS; i++) w[i] = w[i] ^ cmac[i];
		}

		if(idx == packetLen){
			// this is the last block and no padding was needed
			for(i=0; i<CMAC_DS; i++) w[i] = w[i] ^ k1[i];
		}
		if(idx > packetLen){
			// this is the last block and padding was added
			for(i=0; i<CMAC_DS; i++) w[i] = w[i] ^ k2[i];
		}

		chacha->EncryptBlock(ctx, w, CMAC_DS);

		for(i=0; i<CMAC_DS; i++) cmac[i] = w[i];
	}
}
// -----------------------------------------------------------------------------

} /* namespace */
