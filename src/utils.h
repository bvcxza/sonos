#pragma once

#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace sonos
{

bool replaceAll(std::string& inout, const std::map<std::string_view,std::string_view>& map);

std::vector<uint8_t> random(uint16_t length);

uint64_t timestamp();

std::pair<std::string,std::string> split_pair(const std::string& str, char delim);

std::string sha256(const uint8_t* data, size_t size);

template <class T>
std::string sha256(const T& msg)
{
	return sha256(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());
}

/** Convert from one power-of-2 number base to another. */
template<int frombits, int tobits, bool pad = false>
std::vector<uint8_t> convertbits(const std::vector<uint8_t>& in)
{
	int acc = 0;
	int bits = 0;
	constexpr int maxv = (1 << tobits) - 1;
	constexpr int max_acc = (1 << (frombits + tobits - 1)) - 1;
	std::vector<uint8_t> out;
	for (int value : in)
	{
		acc = ((acc << frombits) | value) & max_acc;
		bits += frombits;
		while (bits >= tobits)
		{
			bits -= tobits;
			out.push_back((acc >> bits) & maxv);
		}
	}
	if constexpr (pad)
	{
		if (bits) out.push_back((acc << (tobits - bits)) & maxv);
		return out;
	}
	if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
		throw std::runtime_error("Invalid bit conversion");
	}
	return out;
}

}
