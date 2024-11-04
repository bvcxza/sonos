#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace sonos
{

class key
{
public:
	key() = default;
	key(const std::string& key);
	key(std::vector<uint8_t>&& data);
	auto size() const { return m_data.size(); }
	auto data() const { return m_data.data(); }
	auto data() { m_data.resize(32); return m_data.data(); }
	std::string to_hex() const;
	std::string to_bech32(const std::string& hrp) const;
protected:
	std::vector<uint8_t> m_data;
};

}
