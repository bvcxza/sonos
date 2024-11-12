#include <gtest/gtest.h>

#include <keypair.h>
#include <utils.h>

TEST(KeyPairTest, FromBech32)
{
	sonos::keypair keypair { "nsec182frsrgf50ktzfjj7jk4d9374e8ydqjcaugnvrhfnf4lyuca5ans90fyhf" };
	EXPECT_EQ("db268f11cc8edc105dab376fd9b790103058606f0719f818f96a6b986909341c", keypair.pub().to_hex());
}

TEST(KeyPairTest, Hash)
{
	sonos::keypair keypair { "nsec182frsrgf50ktzfjj7jk4d9374e8ydqjcaugnvrhfnf4lyuca5ans90fyhf" };
	// [0,<pubkey, as a lowercase hex string>,<created_at, as a number>,<kind, as a number>,<tags, as an array of arrays of non-null strings>,<content, as a string>]
	std::string msg = R"([0,"db268f11cc8edc105dab376fd9b790103058606f0719f818f96a6b986909341c",1729681424,1,[],"hi nostr"])";
	EXPECT_EQ("1ce54ff78da19d02dae3b6a7dd4fc3581b76e660e83c36ceda4becb4432a128d", sonos::sha256(msg));
}

TEST(KeyPairTest, HashAndSign)
{
	sonos::keypair keypair { "nsec182frsrgf50ktzfjj7jk4d9374e8ydqjcaugnvrhfnf4lyuca5ans90fyhf" };
	std::string msg = R"([0,"db268f11cc8edc105dab376fd9b790103058606f0719f818f96a6b986909341c",1729681424,1,[],"hi nostr"])";
	auto hash = sonos::sha256(msg);
	ASSERT_EQ(64, hash.size());
	auto signature = keypair.sign(hash);
	ASSERT_EQ(128, signature.size());
	EXPECT_TRUE(keypair.pub().verify(signature, hash));
}

TEST(KeyPairTest, Sign)
{
	sonos::keypair keypair { "nsec182frsrgf50ktzfjj7jk4d9374e8ydqjcaugnvrhfnf4lyuca5ans90fyhf" };
	std::string msg = "3867e83395d0885f6898111d595fa9fff1f20352deea4f1994fed0ec1578e5c1";
	auto signature = keypair.sign(msg);
	ASSERT_EQ(128, signature.size());
	EXPECT_TRUE(keypair.pub().verify(signature, msg));
}
