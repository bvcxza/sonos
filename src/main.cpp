
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "commands/convert_cmd.h"
#include "commands/event_cmd.h"
#include "commands/keypair_cmd.h"

//------------------------------------------------------------------------------

void show_help(auto&& commands)
{
	std::cerr << "Commands:\n";
	for (auto&& [name,cmd] : commands)
	{
		std::cerr << '\n' << name << ":\n";
		std::cerr << cmd->help();
	}
	std::cerr << std::endl;
};

int main(int argc, char* argv[])
{
	std::map<std::string, std::shared_ptr<sonos::command>> commands = {
		{"convert", std::make_shared<sonos::convert_cmd>()},
		{"event", std::make_shared<sonos::event_cmd>()},
		{"keypair", std::make_shared<sonos::keypair_cmd>()}//,
	};

	// Check command line arguments.
	if (argc < 2)
	{
		show_help(commands);
		return EXIT_FAILURE;
	}

	if (!commands.contains(argv[1]))
	{
		std::cerr << argv[0] << " Command not found: " << argv[1] << '\n';
		show_help(commands);
		return EXIT_FAILURE;
	}
	auto cmd = commands[argv[1]];
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
