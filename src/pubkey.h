#pragma once

#include "key.h"

namespace sonos
{

class pubkey : public key
{
public:
	pubkey() = default;
	pubkey(const std::string& pubkey);
	pubkey(std::vector<uint8_t>&& data);
	bool verify(const std::string& signature, const std::string& msg_hash) const;
};

}
