#include "gui_cmd.h"

#include <QApplication>

#include "../gui/main_window.h"

namespace sonos
{

std::string gui_cmd::help() const
{
	return R"(
		Open sonos graphic user interface.
		Usage: sonos gui
	)";
}

bool gui_cmd::execute(int argc, char* argv[])
{
	if (argc != 2) return false;

	QApplication a(argc, argv);
	gui::main_window mw;
	mw.show();
	auto ret = a.exec();
	return ret == 0;
}

}
