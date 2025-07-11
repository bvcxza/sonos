#pragma once

#include <functional>
#include <utility>

#include <QString>
#include <QThread>
#include <QVariant>

namespace sonos::gui
{

class worker : public QThread
{
	Q_OBJECT
public:
	template <class ProcessFunction>
	explicit worker(ProcessFunction&& pf, QObject *parent = nullptr) :
		QThread(parent),
		m_processFunction(std::forward<ProcessFunction>(pf))
	{
		connect(this, &QThread::finished, this, &QObject::deleteLater);
	}

	~worker() override = default;

signals:
	void done(const QVariant& result);
	void fail(const QString& msg, const QString& type);

private:
	void run() override;

	std::function<QVariant()> m_processFunction;
};

}
