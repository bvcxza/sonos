#pragma once

#include <string>

namespace sonos
{

class command
{
public:
	virtual std::string help() const = 0;
	virtual bool execute(int argc, char* argv[]) = 0;
};

}
