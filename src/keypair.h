#pragma once

#include <functional>
#include <memory>

#include <secp256k1.h>

#include "key.h"

namespace sonos
{

class keypair
{
public:
	keypair(key&& secret_key);
	keypair(std::vector<uint8_t>&& secret_data);
	keypair(const std::string& secret);
	const key& sec() const { return m_sec; };
	const key& pub() const { return m_pub; };
private:
	std::unique_ptr<secp256k1_context, std::function<void(secp256k1_context*)>> m_secp256k1_ctx;
	key m_sec, m_pub;
};

}
