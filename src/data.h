#pragma once

#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

namespace sonos
{

template <class T = uint8_t, std::size_t N = 32>
class data
{
public:
	data() = default;
	data(const std::vector<T>& d) : m_d(d) {}
	data(std::vector<T>&& d) : m_d(std::move(d)) {}

	bool empty() const { return m_d.empty(); }
	auto size() const { return m_d.size(); }
	auto begin() const { return m_d.begin(); }
	auto end() const { return m_d.end(); }
	auto get() const { return m_d.data(); }
	auto get() { m_d.resize(N); return m_d.data(); }

public:
	std::string to_hex() const
	{
		std::ostringstream ss;
		ss << std::hex << std::setfill('0');
		for (auto&& d : m_d) ss << std::setw(2) << static_cast<unsigned short>(d);
		return ss.str();
	}

	/** Convert from one power-of-2 number base to another. */
	template<int frombits, int tobits, bool pad = false>
	data<T,N> convertbits() const
	{
		int acc = 0;
		int bits = 0;
		constexpr int maxv = (1 << tobits) - 1;
		constexpr int max_acc = (1 << (frombits + tobits - 1)) - 1;
		data<T,N> out;
		for (int value : m_d)
		{
			acc = ((acc << frombits) | value) & max_acc;
			bits += frombits;
			while (bits >= tobits)
			{
				bits -= tobits;
				out.m_d.push_back((acc >> bits) & maxv);
			}
		}
		if constexpr (pad)
		{
			if (bits) out.m_d.push_back((acc << (tobits - bits)) & maxv);
			return out;
		}
		if (bits >= frombits || ((acc << (tobits - bits)) & maxv)) {
			throw std::runtime_error("Invalid bit conversion");
		}
		return out;
	}

private:
	std::vector<T> m_d;
};

}
