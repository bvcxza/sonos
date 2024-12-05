#include "utils.h"

#include <chrono>
#include <sstream>
#include <random>
#include <utility>
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

std::vector<uint8_t> random(uint16_t length)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distrib(0, 255);
	std::vector<uint8_t> out;
	out.reserve(length);
	for (uint16_t i = 0; i < length; ++i)
		out.push_back(static_cast<uint8_t>(distrib(gen)));

	return out;
}

std::string sha256(const uint8_t* data, size_t size)
{
	std::vector<uint8_t> hash;
	hash.resize(32);
	SHA256(data, size, hash.data());
	std::string to;
	boost::algorithm::hex_lower(hash.begin(), hash.end(), std::back_inserter(to));
	return to;
}

uint64_t timestamp()
{
	const auto p1 = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count();
}

std::pair<std::string,std::string> split_pair(const std::string& str, char delim)
{
	std::istringstream ss {str};
	std::pair<std::string,std::string> str_pair;
	std::getline(ss, str_pair.first, delim);
	std::getline(ss, str_pair.second);
	return str_pair;
}

}
