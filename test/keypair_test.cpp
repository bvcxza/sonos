#include <gtest/gtest.h>

#include <keypair.h>

TEST(KeyPairTest, FromBech32)
{
	sonos::keypair keypair { "nsec1ceu08vm0wjxa2u42x2z62jwwupx7zg56c8z6q8mq6audh9eg0gps4w7064" };
  EXPECT_EQ("f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f", keypair.pub().to_hex());
}

