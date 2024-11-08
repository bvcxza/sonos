
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "commands/convert_cmd.h"
#include "commands/event_cmd.h"
#include "commands/keypair_cmd.h"

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	// Check command line arguments.
	if (argc < 2)
	{
		std::cerr << "EXIT_FAILURE\n";
		return EXIT_FAILURE;
	}

	std::map<std::string, std::shared_ptr<sonos::command>> commands = {
		{"convert", std::make_shared<sonos::convert_cmd>()},
		{"event", std::make_shared<sonos::event_cmd>()},
		{"keypair", std::make_shared<sonos::keypair_cmd>()}//,
	};

	auto cmd = commands[argv[1]];
	if (!cmd)
	{
		std::cerr << argv[0] << " Error: " << "TODO help" << std::endl;
		return EXIT_FAILURE;
	}
	try
	{
		if (!cmd->execute(argc, argv))
		{
			std::cerr << argv[0] << " Error\n" << cmd->help() << std::endl;
			return EXIT_FAILURE;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << argv[0] << " Error: " << e.what() << '\n' << cmd->help() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << argv[0] << " EXIT_SUCCESS\n";
	return EXIT_SUCCESS;
}
