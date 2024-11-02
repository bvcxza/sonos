#include "key.h"

#include <cassert>
#include <memory>

#include <bech32.h>

#include "key_error.h"

namespace sonos
{

key::key(const std::string& key)
{
	auto dec_res = bech32::decode(key);
	if (dec_res.encoding != bech32::Encoding::BECH32)
		throw key_error("key encoding is not BECH32");

	data bech32_data (std::move(dec_res.data));
	m_data = bech32_data.convertbits<5, 8>();
	assert(size() == 32);
	assert(to_hex().size() == (size() * 2));
}

std::string key::to_hex() const
{
	return m_data.to_hex();
}

}
