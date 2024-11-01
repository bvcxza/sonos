#include "keypair.h"

#include <cassert>

#include "utils.h"

namespace sonos
{

keypair::keypair(const std::string& secret)
	: m_secp256k1_ctx(secp256k1_context_create(SECP256K1_CONTEXT_NONE), &secp256k1_context_destroy)
	, m_sec(secret)
{
	secp256k1_xonly_pubkey pubkey;
	assert(secp256k1_context_randomize(m_secp256k1_ctx.get(), random(32).data()));
	assert(secp256k1_keypair_create(m_secp256k1_ctx.get(), &m_secp256k1_keypair, m_sec.get()));
	assert(secp256k1_keypair_xonly_pub(m_secp256k1_ctx.get(), &pubkey, nullptr, &m_secp256k1_keypair));
	assert(secp256k1_xonly_pubkey_serialize(m_secp256k1_ctx.get(), m_pub.get(), &pubkey));
}

}
