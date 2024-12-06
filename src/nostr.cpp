#include "nostr.h"

#include <cassert>
#include <map>

#include <boost/json.hpp>

#include "utils.h"

namespace sonos
{

std::string nostr::make_event(uint16_t kind, const std::string& content, const std::string& tags) const
{
	// [0,<pubkey, as a lowercase hex string>,<created_at, as a number>,<kind, as a number>,<tags, as an array of arrays of non-null strings>,<content, as a string>]
	// TODO tags
	std::string serial_event = R"([0,"${pubkey}",${created_at},${kind},[${tags}],"${content}"])";
	std::string event = R"(["EVENT",{
	"id":"${id}",
	"pubkey":"${pubkey}",
	"created_at":${created_at},
	"kind":${kind},
	"tags":[${tags}],
	"content":"${content}",
	"sig":"${sig}"
}])";
	auto created_at = std::to_string(timestamp());
	bool result = replaceAll(serial_event, {
		{"${pubkey}", m_keypair.pub().to_hex()},
		{"${created_at}", created_at},
		{"${kind}", std::to_string(kind)},
		{"${tags}", tags},
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
		{"${tags}", tags},
		{"${content}", content},
		{"${sig}", sig}
	});
	assert(result);
	return event;
}

bool nostr::verify(const std::string& event) const
{
	using namespace boost::json;

	value vjson = parse(event);
	auto&& evt_array = vjson.as_array();
	assert(evt_array.size() == 2);
	assert(value_to<std::string>(evt_array[0]) == "EVENT");
	auto&& evt = evt_array[1].as_object();
	return m_keypair.pub().verify(value_to<std::string>(evt["sig"]), value_to<std::string>(evt["id"]));
}

}
