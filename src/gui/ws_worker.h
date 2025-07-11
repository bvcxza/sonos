#pragma once

#include <string>
#include <vector>

#include <QString>

#include "worker.h"

namespace sonos::gui
{

class ws_worker : public worker
{
	Q_OBJECT
public:
	ws_worker(const QString& txt, const std::vector<std::string>& hosts, QObject* parent = nullptr);
};

}
