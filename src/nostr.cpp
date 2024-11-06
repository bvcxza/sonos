#include "nostr.h"

#include <cassert>
#include <map>

#include "utils.h"

namespace sonos
{

std::string nostr::make_event(uint16_t kind, const std::string& content) const
{
	// [0,<pubkey, as a lowercase hex string>,<created_at, as a number>,<kind, as a number>,<tags, as an array of arrays of non-null strings>,<content, as a string>]
	// TODO tags
	std::string serial_event = R"([0,"${pubkey}",${created_at},${kind},[],"${content}"])";
	std::string event = R"({
	"id":"${id}",
	"pubkey":"${pubkey}",
	"created_at":${created_at},
	"kind":${kind},
	"tags":[],
	"content":"${content}",
	"sig":"${sig}"
})";
	auto created_at = std::to_string(timestamp());
	bool result = replaceAll(serial_event, {
		{"${pubkey}", m_keypair.pub().to_hex()},
		{"${created_at}", created_at},
		{"${kind}", std::to_string(kind)},
		{"${content}", content}
	});
	assert(result);
	auto id = sha256(serial_event);
	auto sig = m_keypair.sign(id);
	result = replaceAll(event, {
		{"${id}", id},
		{"${pubkey}", m_keypair.pub().to_hex()},
		{"${created_at}", created_at},
		{"${kind}", std::to_string(kind)},
		{"${content}", content},
		{"${sig}", sig}
	});
	assert(result);
	return event;
}

}
