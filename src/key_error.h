#pragma once

#include <stdexcept>

namespace sonos
{

class key_error : public std::runtime_error
{
public:
	key_error(const std::string& what_arg) : std::runtime_error(what_arg) {}
};

}
