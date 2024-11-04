#include <gtest/gtest.h>

#include <keypair.h>
#include <utils.h>

TEST(KeyPairTest, FromBech32)
{
	sonos::keypair keypair { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
  EXPECT_EQ("f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f", keypair.pub().to_hex());
}

TEST(KeyPairTest, Hash)
{
	sonos::keypair keypair { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
	// [0,<pubkey, as a lowercase hex string>,<created_at, as a number>,<kind, as a number>,<tags, as an array of arrays of non-null strings>,<content, as a string>]
	std::string msg = R"([0,"f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f",1729681424,1,[],"hi nostr"])";
  EXPECT_EQ("3867e83395d0885f6898111d595fa9fff1f20352deea4f1994fed0ec1578e5c1", sonos::hash32(msg));
}

TEST(KeyPairTest, HashAndSign)
{
	sonos::keypair keypair { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
	std::string msg = R"([0,"f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f",1729681424,1,[],"hi nostr"])";
	auto hash = sonos::hash32(msg);
	ASSERT_EQ(64, hash.size());
	auto signature = keypair.sign(hash);
	ASSERT_EQ(128, signature.size());
	EXPECT_TRUE(keypair.pub().verify(signature, hash));
}

TEST(KeyPairTest, Sign)
{
	sonos::keypair keypair { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
	std::string msg = "3867e83395d0885f6898111d595fa9fff1f20352deea4f1994fed0ec1578e5c1";
	auto signature = keypair.sign(msg);
	ASSERT_EQ(128, signature.size());
	EXPECT_TRUE(keypair.pub().verify(signature, msg));
}
