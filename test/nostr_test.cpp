#include <gtest/gtest.h>

#include <nostr.h>

TEST(NostrTest, MakeEevent)
{
	std::string expected_event = R"({
	"id":"${id}",
	"pubkey":"${pubkey}",
	"created_at":${created_at},
	"kind":${kind},
	"tags":[],
	"content":"content as string",
	"sig":"${sig}"
})";
  sonos::nostr nostr { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064"};
  EXPECT_EQ(expected_event, nostr.make_event(1, "content as string"));
}

