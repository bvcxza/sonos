#pragma once

#include "../command.h"

namespace sonos
{

class req_cmd final : public command
{
public:
	std::string help() const override;
	bool execute(int argc, char* argv[]) override;
};

}
