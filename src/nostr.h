#pragma once

#include <memory>
#include <string>

#include "keypair.h"

namespace sonos
{

class nostr
{
public:
	nostr(const std::string& secret) : m_keypair(secret) {}
	nostr(keypair&& keypair) : m_keypair(std::move(keypair)) {}

	std::string make_event(uint16_t kind, const std::string& content) const;

private:
	keypair m_keypair;
};

}
