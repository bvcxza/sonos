#include "main_window.h"

#include <iostream>
#include <vector>

#include <QPushButton>
#include <QStatusBar>
#include <QThread>

#include "worker.h"

namespace sonos::gui
{

namespace { auto threadId = QThread::currentThreadId; }

main_window::main_window()
{
	std::cout << threadId() << " - main_window\n";
	auto button = new QPushButton("Run", this);
	connect(button, &QPushButton::clicked, this, [this]{
		statusBar()->showMessage("Processing...");
		auto wk = new worker([]() -> QVariant {
			std::cout << threadId() << " - processing..." << std::endl;
			QThread::sleep(5);
			//throw std::runtime_error("erro em tempo de execução");
			return "Ok";
		});
		connect(wk, &worker::done, this, [this](auto&& result){
			std::cout << threadId() << " - result: " << result.toString().toStdString() << std::endl;
			statusBar()->showMessage("Result: " + result.toString());
		});
		connect(wk, &worker::fail, this, [this](auto&& msg, auto&& type){
			std::cout << threadId() << " - error: " << msg.toStdString() << " [" << type.toStdString() << ']' << std::endl;
			statusBar()->showMessage(QString("Error: ") + msg);
		});
		wk->start();
	});
	setCentralWidget(button);
	statusBar()->showMessage(QObject::tr("Ready"), 3000);
}

}
