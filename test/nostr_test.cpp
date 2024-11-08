#include <gtest/gtest.h>

#include <boost/json.hpp>

#include <nostr.h>
#include <utils.h>

TEST(NostrTest, MakeEvent)
{
	using namespace boost::json;

	std::string expected_event = R"(["EVENT",{
	"id":"${id}",
	"pubkey":"${pubkey}",
	"created_at":${created_at},
	"kind":1,
	"tags":[],
	"content":"content as string",
	"sig":"${sig}"
}])";

  sonos::nostr nostr { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
  auto&& event = nostr.make_event(1, "content as string");
	value vjson = parse(event);
	auto&& evt_array = vjson.as_array();
	assert(evt_array.size() == 2);
	assert(value_to<std::string>(evt_array[0]) == "EVENT");
	auto&& evt = evt_array[1].as_object();
	bool result = sonos::replaceAll(expected_event, {
		{"${id}", value_to<std::string>(evt["id"])},
		{"${pubkey}", value_to<std::string>(evt["pubkey"])},
		{"${created_at}", std::to_string(value_to<uint64_t>(evt["created_at"]))},
		{"${sig}", value_to<std::string>(evt["sig"])}
	});
	ASSERT_TRUE(result);

  ASSERT_EQ(expected_event, event);
}

TEST(NostrTest, Verify)
{
  sonos::nostr nostr { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
  EXPECT_TRUE(nostr.verify(nostr.make_event(1, "content as string")));
}

