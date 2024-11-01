#include <gtest/gtest.h>

#include <key.h>

TEST(KeyTest, FromBech32)
{
	sonos::key key { "npub10elfcs4fr0l0r8af98jlmgdh9c8tcxjvz9qkw038js35mp4dma8qzvjptg" };
  EXPECT_EQ("7e7e9c42a91bfef19fa929e5fda1b72e0ebc1a4c1141673e2794234d86addf4e", key.to_hex());
}

