#include "utils.h"

#include <random>
#include <vector>

#include <boost/algorithm/hex.hpp>
#include <openssl/sha.h>

namespace sonos
{

bool replaceAll(std::string& inout, const std::map<std::string_view,std::string_view>& map)
{
	bool result = false;
	for (auto [what,with] : map)
		for (std::string::size_type pos{};
		     inout.npos != (pos = inout.find(what.data(), pos, what.length()));
		     pos += with.length(), result = true)
			inout.replace(pos, what.length(), with.data(), with.length());

	return result;
}

std::vector<uint8_t> random(size_t length)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, 255);
	std::vector<uint8_t> out;
	out.reserve(length);
	for (auto i = 0; i < length; ++i)
		out.push_back(static_cast<uint8_t>(distrib(gen)));

	return out;
}

std::string hash32(const std::string& msg)
{
	std::vector<uint8_t> msg_hash;
	msg_hash.resize(32);
	SHA256(reinterpret_cast<const uint8_t*>(msg.data()), msg.size(), msg_hash.data());
	std::string to;
	boost::algorithm::hex_lower(msg_hash.begin(), msg_hash.end(), std::back_inserter(to));
	return to;
}

}
