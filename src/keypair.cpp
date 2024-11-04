#include "keypair.h"

#include <cassert>

#include <boost/algorithm/hex.hpp>
#include <secp256k1_schnorrsig.h>

#include "utils.h"

namespace sonos
{

keypair::keypair(key&& secret_key)
	: m_ctx(secp256k1_context_create(SECP256K1_CONTEXT_NONE), &secp256k1_context_destroy)
	, m_sec(secret_key)
{
	secp256k1_xonly_pubkey pubkey;
	assert(secp256k1_context_randomize(m_ctx.get(), random(32).data()));
	assert(secp256k1_keypair_create(m_ctx.get(), &m_keypair, m_sec.data()));
	assert(secp256k1_keypair_xonly_pub(m_ctx.get(), &pubkey, nullptr, &m_keypair));
	assert(secp256k1_xonly_pubkey_serialize(m_ctx.get(), m_pub.data(), &pubkey));
}

keypair::keypair(std::vector<uint8_t>&& secret_data) : keypair(key(std::move(secret_data))) {}

keypair::keypair(const std::string& secret) : keypair(key(secret)) {}

std::string keypair::sign(const std::string& msg_hash) const
{
	assert(msg_hash.size() == 64);
	std::vector<uint8_t> hash;
	boost::algorithm::unhex(msg_hash, std::back_inserter(hash));
	std::vector<uint8_t> signature;
	signature.resize(64);
	assert(secp256k1_schnorrsig_sign32(m_ctx.get(), signature.data(), hash.data(), &m_keypair, random(32).data()));
	std::string to;
	boost::algorithm::hex_lower(signature.begin(), signature.end(), std::back_inserter(to));
	return to;
}

}
