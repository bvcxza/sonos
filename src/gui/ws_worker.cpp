#include "ws_worker.h"

#include <exception>
#include <iostream>
#include <utility>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include "../net_utils.h"

namespace sonos::gui
{

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace
{

auto threadId = QThread::currentThreadId;

QVariant process(const QString& txt, const std::vector<std::string>& hosts)
{
	std::cout << threadId() << " - processing '" << txt.toStdString() << "'..." << std::endl;
	//
	net::io_context ioc;
	ssl::context ctx{ssl::context::tlsv12_client};
	ctx.set_default_verify_paths();
	tcp::resolver resolver{ioc};

	bool ok = true;
	for (auto&& address : hosts)
	{
		auto&& [host, port] = split_pair(address, ':');

		try
		{
			websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
			connect(beast::get_lowest_layer(ws), resolver, tcp::resolver::query{host, port});

			if (!SSL_set_tlsext_host_name(ws.next_layer().native_handle(), host.c_str()))
				throw beast::system_error(
					beast::error_code(
						static_cast<int>(::ERR_get_error()),
						net::error::get_ssl_category()),
					"Failed to set SNI Hostname");


			ws.next_layer().handshake(ssl::stream_base::client);

			ws.set_option(websocket::stream_base::decorator(
			[](websocket::request_type& req)
			{
				req.set(http::field::user_agent,
					std::string(BOOST_BEAST_VERSION_STRING) + " sonos");
			}));

			ws.handshake(address, "/");
			ws.write(net::buffer(txt.toStdString()));
			beast::flat_buffer buffer;
			ws.read(buffer);
			try{
			ws.close(websocket::close_code::normal);
			}catch(...){ /*socks5 throws "Transport endpoint is not connected"*/ }

			return QString::fromStdString(beast::buffers_to_string(buffer.data()));
		}
		catch (const std::exception& e)
		{
			ok = false;
			std::cerr << "Error [" << address << "]: " << e.what() << std::endl;
			continue;
		}
	}

	if (!ok) throw std::runtime_error("fail to send message");

	return {};
}

}

ws_worker::ws_worker(const QString& txt, const std::vector<std::string>& hosts, QObject* parent)
	: worker(std::bind(&process, txt, hosts), parent) {}
}
