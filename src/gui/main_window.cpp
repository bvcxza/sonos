#include "main_window.h"

#include <iostream>
#include <vector>

#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QThread>
#include <QToolBar>
#include <QVBoxLayout>

#include "ws_worker.h"

namespace sonos::gui
{

namespace { auto threadId = QThread::currentThreadId; }

main_window::main_window(const std::vector<std::string>& hosts)
{
	std::cout << threadId() << " - main_window\n";
	setMinimumSize(800, 600);
	auto toolBar = new QToolBar(this);
	toolBar->addAction("User", [this]{
		setCentralWidget(new QLabel("Hi user!!", this));
	});
	toolBar->addAction("Message", [this,hosts]{
		auto outMsg = new QTextEdit(this);
		outMsg->setPlaceholderText("Text to send");
		auto inMsg = new QTextEdit(this);
		inMsg->setPlaceholderText("Text to receive");
		inMsg->setReadOnly(true);
		auto button = new QPushButton("Run", this);
		connect(button, &QPushButton::clicked, this, [this,outMsg,inMsg,hosts]{
			statusBar()->showMessage("Processing...");
			auto wk = new ws_worker(outMsg->toPlainText(), hosts, this);
			connect(wk, &ws_worker::done, this, [this,inMsg](auto&& result){
				std::cout << threadId() << " - result: " << result.toString().toStdString() << std::endl;
				statusBar()->showMessage("Ok");
				inMsg->setPlainText(result.toString());
			});
			connect(wk, &ws_worker::fail, this, [this](auto&& msg, auto&& type){
				std::cout << threadId() << " - error: " << msg.toStdString() << " [" << type.toStdString() << ']' << std::endl;
				statusBar()->showMessage(QString("Error: ") + msg);
			});
			wk->start();
		});

		auto box = new QVBoxLayout(new QWidget);
		box->addWidget(outMsg);
		box->addWidget(inMsg);
		box->addWidget(button);
		setCentralWidget(box->parentWidget());
	});
	addToolBar(Qt::LeftToolBarArea, toolBar);
	statusBar()->showMessage(QObject::tr("Ready"), 3000);
}

}
