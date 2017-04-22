/**
 * created 2016.07.03 by Calin Radoni
 */

#include "PAW_CommProtocol.h"

#include <cpu_Entropy.h>

namespace PAx5CommProtocol {

// -----------------------------------------------------------------------------

PAW_CommProtocol::PAW_CommProtocol()
{
	cmac.chacha = &chacha;

	nodeAddress = AddressClass::Address_NONE;

	packetRX = &packetIn[CP_PACKET_RAW_OFFSET];
	packetTX = &packetOut[CP_PACKET_RAW_OFFSET];

	packetRX[0] = 0;
	packetTX[0] = 0;
}

// -----------------------------------------------------------------------------

bool PAW_CommProtocol::CompareHashInConstantTime(uint8_t hashStartIdx)
{
	uint8_t i, idx;
	uint8_t res;

	res = 0;
	idx = hashStartIdx;
	for(i = 0; i < CP_PACKET_HASH_LEN; i++)
		res |= (packetRX[idx++] ^ cmac.cmac[i]);
	return res == 0;
}

// -----------------------------------------------------------------------------

uint32_t PAW_CommProtocol::BP2DW(uint8_t* in)
{
	if(in == NULL) return 0;

	// NOTE: reinterpret_cast generated a HardFault, so I have choosed this solution

	uint32_t val;

	val  = in[0]; val = val << 8;
	val |= in[1]; val = val << 8;
	val |= in[2]; val = val << 8;
	val |= in[3];
	return val;
}

void PAW_CommProtocol::DW2BP(uint32_t val32, uint8_t* out)
{
	if(out == NULL) return;

	uint32_t val = val32;

	out[3] = (uint8_t)(val & 0x000000FF); val = val >> 8;
	out[2] = (uint8_t)(val & 0x000000FF); val = val >> 8;
	out[1] = (uint8_t)(val & 0x000000FF); val = val >> 8;
	out[0] = (uint8_t)(val & 0x000000FF);
}

// -----------------------------------------------------------------------------

void PAW_CommProtocol::GetNewSessionID(void)
{
	currentSessionID++;
	if(currentSessionID == 0)
		currentSessionID++;
}

void PAW_CommProtocol::NewNonce(void)
{
	currentNonce[0]++;
	if(currentNonce[0] == 0){
		currentNonce[1]++;
		if(currentNonce[1] == 0)
			currentNonce[0]++;	// avoid nonce == 0
	}
}

// -----------------------------------------------------------------------------

void PAW_CommProtocol::SetDefaultKey(CryptoContext* ctx)
{
	if(ctx == NULL) return;

	ctx->use256bitsEncKey = false;

	ctx->key[0] = 0x50417835; // PAx5
	ctx->key[1] = 0x11223344;
	ctx->key[2] = 0x55667788;
	ctx->key[3] = 0x99AABBCC;
}

// -----------------------------------------------------------------------------

void PAW_CommProtocol::Initialize(uint8_t thisNodeAddress)
{
	currentSessionID = PAx5::entropy.Get32bits();
	currentNonce[0]  = PAx5::entropy.Get32bits();
	currentNonce[1]  = PAx5::entropy.Get32bits();
	PAx5::entropy.Disable();

	nodeAddress = thisNodeAddress;
}

void PAW_CommProtocol::CreateSessionKey(PAW_TimedSlot* ts)
{
	if(ts == NULL) return;

	// client nonce is in ts->sessionEncCtx.key[0 and 1]
	// server nonce is in ts->sessionEncCtx.key[2 and 3]

	// FIXME PAW_CommProtocol::CreateSessionKey rebuild this function
}

void PAW_CommProtocol::BuildPacketHeader(CryptoContext* ctx, uint8_t dst, uint8_t flags, uint8_t dataLength, uint32_t sessionID)
{
	if(ctx == NULL) return;

	if(dataLength > CP_PACKET_MAX_DATA_LEN) return;
	packetTX[0] = dataLength + CP_PACKET_OVERHEAD;
	packetTX[1] = dst;
	packetTX[2] = nodeAddress;
	packetTX[3] = (flags & CP_PACKET_HDR_FLAGS) | 1;

	NewNonce();

	DW2BP(sessionID,       &packetTX[CP_PACKET_HDR_ID_POS]);
	DW2BP(currentNonce[0], &packetTX[CP_PACKET_HDR_NONCE0_POS]);
	DW2BP(currentNonce[1], &packetTX[CP_PACKET_HDR_NONCE1_POS]);

	// after building the header, set the nonce for current encryption context

	ctx->cnonce[0] = BP2DW(&packetTX[0]);
	ctx->cnonce[1] = sessionID;
	ctx->cnonce[2] = currentNonce[0];
	ctx->cnonce[3] = currentNonce[1];
}

// -----------------------------------------------------------------------------

void PAW_CommProtocol::CreateRequestPacket(PAW_TimedSlot* ts, CryptoContext* ctx)
{
	uint32_t randValH, randValL;

	if(ts == NULL) return;
	if(ctx == NULL) return;

	GetNewSessionID();
	ts->SetSessionID(currentSessionID);
	BuildPacketHeader(ctx, ts->peerAddress, CP_PACKET_HDR_REQ, 8, ts->GetSessionID());

	// generate random values ...
	do {
		randValH = PAx5::entropy.Get32bits();
		randValL = PAx5::entropy.Get32bits();
	} while((randValH == 0) || (randValL == 0));
	PAx5::entropy.Disable();

	// ... store them ...
	ts->InitTimeSlot(CP_TIME_WAIT_REQACK, randValH, randValL);

	// ... and add them as payload
	DW2BP(randValH, &packetTX[CP_PACKET_HEADER_LEN]);
	DW2BP(randValL, &packetTX[CP_PACKET_HEADER_LEN + 4]);

	EncryptAndSign(ctx);
}

void PAW_CommProtocol::CreateReqAckPacket(PAW_TimedSlot* ts, CryptoContext* ctx)
{
	uint32_t randValH, randValL;

	if(ts == NULL) return;
	if(ctx == NULL) return;

	BuildPacketHeader(ctx, ts->peerAddress, CP_PACKET_HDR_REQ | CP_PACKET_HDR_ACK, 8, ts->GetSessionID());

	// generate random values ...
	do {
		randValH = PAx5::entropy.Get32bits();
		randValL = PAx5::entropy.Get32bits();
	} while((randValH == 0) || (randValL == 0));
	PAx5::entropy.Disable();

	// ... store them ...
	ts->UpdateTimeSlotReqAck(CP_TIME_WAIT_REQACK, randValH, randValL);

	// ... then and add them as payload
	DW2BP(randValH, &packetTX[CP_PACKET_HEADER_LEN]);
	DW2BP(randValL, &packetTX[CP_PACKET_HEADER_LEN + 4]);

	EncryptAndSign(ctx);
}

void PAW_CommProtocol::CreateAckPacket(PAW_TimedSlot* ts, uint8_t payloadLength)
{
	if(ts == NULL) return;

	BuildPacketHeader(&(ts->sessionEncCtx), ts->peerAddress, CP_PACKET_HDR_ACK, payloadLength, ts->GetSessionID());

	EncryptAndSign(&(ts->sessionEncCtx));
}

void PAW_CommProtocol::CreateDataPacket(PAW_TimedSlot* ts, uint8_t payloadLength, bool lastPacket)
{
	uint8_t outFlags;

	if(ts == NULL) return;

	outFlags = lastPacket ? 0 : CP_PACKET_HDR_WAIT;

	BuildPacketHeader(&(ts->sessionEncCtx), ts->peerAddress, outFlags, payloadLength, ts->GetSessionID());

	EncryptAndSign(&(ts->sessionEncCtx));
}

// -----------------------------------------------------------------------------

PAW_ChkAddr::CheckAddrResult PAW_CommProtocol::CheckAddresses(void)
{
	if(packetRX[CP_PACKET_HDR_SrcAddr_POS] == AddressClass::Address_NONE){
		if(packetRX[CP_PACKET_HDR_DstAddr_POS] == AddressClass::Address_Gateway)
			return PAW_ChkAddr::srcNone_dstGW;
		return PAW_ChkAddr::wrongCombo;
	}
	if(packetRX[CP_PACKET_HDR_SrcAddr_POS] == AddressClass::Address_Broadcast){
		return PAW_ChkAddr::wrongCombo;
	}

	if(packetRX[CP_PACKET_HDR_DstAddr_POS] == AddressClass::Address_Gateway)   return PAW_ChkAddr::srcOK_dstGW;
	if(packetRX[CP_PACKET_HDR_DstAddr_POS] == nodeAddress)                     return PAW_ChkAddr::srcOK_dstME;
	if(packetRX[CP_PACKET_HDR_DstAddr_POS] == AddressClass::Address_Broadcast) return PAW_ChkAddr::srcOK_dstBcast;

	return PAW_ChkAddr::wrongCombo;
}

// -----------------------------------------------------------------------------

void PAW_CommProtocol::EncryptAndSign(CryptoContext* ctx)
{
	uint8_t i, idx, dataLen;

	if(ctx == NULL) return;

	dataLen  = packetTX[0];
	if(dataLen > CP_PACKET_MAX_LEN)  return;
	if(dataLen < CP_PACKET_OVERHEAD) return;
	dataLen -= CP_PACKET_OVERHEAD;

	// Step 1: Encrypt
	if(dataLen > 0)
		chacha.EncryptBlock(ctx, &packetTX[CP_PACKET_HEADER_LEN], dataLen);

	// Step 2: Sign
	ctx->cnonce[0] = ctx->cnonce[0] ^ 0x0F;
	cmac.Sign(ctx, packetTX, dataLen + CP_PACKET_HEADER_LEN);
	ctx->cnonce[0] = ctx->cnonce[0] ^ 0x0F;

	// Step 3: put the signature in the packet
	idx = dataLen + CP_PACKET_HEADER_LEN;
	for(i = 0; i < CP_PACKET_HASH_LEN; i++)
		packetTX[idx++] = cmac.cmac[i];
}

// -----------------------------------------------------------------------------

bool PAW_CommProtocol::CheckSignatureAndDecrypt(CryptoContext* ctx)
{
	uint32_t dataLen;

	if(ctx == NULL) return false;

	dataLen  = packetRX[0];
	if(dataLen > CP_PACKET_MAX_LEN)  return false;
	if(dataLen < CP_PACKET_OVERHEAD) return false;
	dataLen -= CP_PACKET_OVERHEAD;

	// set ctx->cnonce from packet, derived for signing
	ctx->cnonce[0] = BP2DW(&packetRX[0]);
	ctx->cnonce[1] = BP2DW(&packetRX[CP_PACKET_HDR_ID_POS]);
	ctx->cnonce[2] = BP2DW(&packetRX[CP_PACKET_HDR_NONCE0_POS]);
	ctx->cnonce[3] = BP2DW(&packetRX[CP_PACKET_HDR_NONCE1_POS]);

	// compute signature
	ctx->cnonce[0] = ctx->cnonce[0] ^ 0x0F;
	cmac.Sign(ctx, packetRX, CP_PACKET_HEADER_LEN + dataLen);
	ctx->cnonce[0] = ctx->cnonce[0] ^ 0x0F;

	// compare hashes
	if(!CompareHashInConstantTime(CP_PACKET_HEADER_LEN + dataLen))
		return false; // wrong hash

	// decrypt the packet
	if(dataLen > 0)
		chacha.EncryptBlock(ctx, &packetRX[CP_PACKET_HEADER_LEN], dataLen);

	return true;
}

// -----------------------------------------------------------------------------

} /* namespace */
