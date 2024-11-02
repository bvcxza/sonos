#include "key.h"

#include <cassert>
#include <memory>

#include <boost/algorithm/hex.hpp>
#include <bech32.h>

#include "key_error.h"
#include "utils.h"

namespace sonos
{

key::key(const std::string& key)
{
	if (key.starts_with("nsec1") || key.starts_with("npub1"))
	{
		auto dec_res = bech32::decode(key);
		if (dec_res.encoding != bech32::Encoding::BECH32)
			throw key_error("key encoding is not BECH32");

		m_data = convertbits<5, 8>(dec_res.data);
	}
	else
	{
		boost::algorithm::unhex(key, std::back_inserter(m_data));
	}
	assert(size() == 32);
	assert(to_hex().size() == (size() * 2));
}

std::string key::to_hex() const
{
	std::string to;
	boost::algorithm::hex_lower(m_data.begin(), m_data.end(), std::back_inserter(to));
	return to;

}

std::string key::to_bech32(const std::string& hrp) const
{
	return bech32::encode(hrp, convertbits<8, 5, true>(m_data), bech32::Encoding::BECH32);
}

}
