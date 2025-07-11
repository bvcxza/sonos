#include "gui_cmd.h"

#include <QApplication>

#include "../gui/main_window.h"

namespace sonos
{

std::string gui_cmd::help() const
{
	return R"(
		Open sonos graphic user interface.
		Usage: sonos gui <relay_addresses ...>
	)";
}

bool gui_cmd::execute(int argc, char* argv[])
{
	if (argc < 3) return false;

	QApplication a(argc, argv);
	std::vector<std::string> hosts;
	for (int i = 2; i < argc; ++i) hosts.emplace_back(argv[i]);
	gui::main_window mw (hosts);
	mw.show();
	auto ret = a.exec();
	return ret == 0;
}

}
