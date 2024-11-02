#include "convert_cmd.h"

#include <iostream>

#include "../key.h"

namespace sonos
{

std::string convert_cmd::help() const
{
	return "convert bech32 addresses (npub1 and nsec1) to/from hexadecimal string";
}

bool convert_cmd::execute(int argc, char* argv[])
{
	if (argc < 2) return false;

	key k { argv[2] };
	std::cout << k.to_hex() << std::endl;
	return true;
}

}
