#pragma once

#include "data.h"

namespace sonos
{

class key
{
public:
	key() = default;
	key(const std::string& key);
	auto size() const { return m_data.size(); }
	auto get() const { return m_data.get(); }
	auto get() { return m_data.get(); }
	std::string to_hex() const;
private:
	data<uint8_t, 32> m_data;
};

}
