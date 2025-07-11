#pragma once

#include <string>
#include <vector>

#include <QMainWindow>

namespace sonos::gui
{

class main_window : public QMainWindow
{
public:
	main_window(const std::vector<std::string>& hosts);
};

}
