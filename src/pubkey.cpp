#include "pubkey.h"

#include <cassert>
#include <functional>
#include <memory>

#include <boost/algorithm/hex.hpp>
#include <secp256k1.h>
#include <secp256k1_schnorrsig.h>

#include "key_error.h"

namespace sonos
{

pubkey::pubkey(const std::string& pubkey) : key(pubkey) {}

pubkey::pubkey(std::vector<uint8_t>&& data) : key(std::move(data)) {}

bool pubkey::verify(const std::string& signature, const std::string& msg_hash) const
{
	assert(signature.size() == 64 * 2);
	std::unique_ptr<secp256k1_context, std::function<void(secp256k1_context*)>> ctx (secp256k1_context_create(SECP256K1_CONTEXT_NONE), &secp256k1_context_destroy);
	secp256k1_xonly_pubkey pubkey;
	if (!secp256k1_xonly_pubkey_parse(ctx.get(), &pubkey, m_data.data()))
		throw new key_error("failed parsing the public key");

	std::vector<uint8_t> sig;
	boost::algorithm::unhex(signature, std::back_inserter(sig));
	std::vector<uint8_t> hash;
	boost::algorithm::unhex(msg_hash, std::back_inserter(hash));
	return secp256k1_schnorrsig_verify(ctx.get(), sig.data(), hash.data(), hash.size(), &pubkey);
}

}
