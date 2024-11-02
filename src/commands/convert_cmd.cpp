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
	if (argc < 3) return false;

	std::string keystr = argv[2];
	key k { keystr };
	if (keystr.starts_with("nsec1") || keystr.starts_with("npub1"))
		std::cout << k.to_hex() << std::endl;
	else
	{
		if (argc != 4) return false;
		std::string hrp = argv[3];
		if (hrp.size() != 4) return false;
		std::cout << k.to_bech32(hrp) << std::endl;
	}
	return true;
}

}
