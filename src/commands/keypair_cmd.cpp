#include "keypair_cmd.h"

#include <iostream>

#include "../keypair.h"
#include "../utils.h"

namespace sonos
{

std::string keypair_cmd::help() const
{
	return R"(
		Create or show keypair from hexadecimal/bech32 secret string
		Usage: sonos keypair [secret]
	)";
}

bool keypair_cmd::execute(int argc, char* argv[])
{
	if (argc < 2) return false;

	if (argc == 2) // create a keypair
	{
		keypair kp { random(32) };
		std::cout << kp.pub().to_bech32("npub") << std::endl;
		std::cout << kp.sec().to_bech32("nsec") << std::endl;
	}
	else
	{
		if (argc != 3) return false;
		keypair kp { argv[2] };
		std::cout << kp.pub().to_bech32("npub") << std::endl;
		std::cout << kp.sec().to_bech32("nsec") << std::endl;
	}

	return true;
}

}
