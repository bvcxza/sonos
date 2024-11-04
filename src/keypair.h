#pragma once

#include <functional>
#include <memory>

#include <secp256k1.h>
#include <secp256k1_extrakeys.h>

#include "pubkey.h"

namespace sonos
{

class keypair
{
public:
	keypair(key&& secret_key);
	keypair(std::vector<uint8_t>&& secret_data);
	keypair(const std::string& secret);
	const key& sec() const { return m_sec; };
	const pubkey& pub() const { return m_pub; };
	std::string sign(const std::string& msg_hash) const;
private:
	std::unique_ptr<secp256k1_context, std::function<void(secp256k1_context*)>> m_ctx;
	key m_sec;
	pubkey m_pub;
	secp256k1_keypair m_keypair;
};

}
