#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace sonos
{

bool replaceAll(std::string& inout, const std::map<std::string_view,std::string_view>& map);

std::vector<uint8_t> random(size_t length);

}
