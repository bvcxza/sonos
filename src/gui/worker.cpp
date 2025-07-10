#include "worker.h"

#include <exception>

namespace sonos::gui
{

void worker::run()
{
	try { emit done(m_processFunction()); }
	catch(const std::exception& e) { emit fail(e.what(), typeid(e).name()); }
}

}
