#include "event_cmd.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include "../nostr.h"
#include "../utils.h"

namespace sonos
{

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string event_cmd::help() const
{
	return "create and send a event to nostr relays";
}

bool event_cmd::execute(int argc, char* argv[])
{
	if (argc < 3) return false;

	try
	{
		net::io_context ioc;
		ssl::context ctx{ssl::context::tlsv12_client};
		ctx.set_default_verify_paths();
		tcp::resolver resolver{ioc};
		websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
		std::string host = "nostr.bitcoiner.social";
		//std::string host = "echo.websocket.org";
		const char* port = "443";
		auto const results = resolver.resolve(host, port);
		auto ep = net::connect(beast::get_lowest_layer(ws), results);

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

		nostr nstr { argv[2] };
		std::string event = nstr.make_event(1, "hi nostr!!");
		host += ':' + std::to_string(ep.port());
		std::cout << "handshaking to " << host << std::endl;
		ws.handshake(host, "/");
		ws.write(net::buffer(event));
		std::cout << "write: " << event << std::endl;
		beast::flat_buffer buffer;
		ws.read(buffer);
		ws.close(websocket::close_code::normal);
		std::cout << "read: " << beast::make_printable(buffer.data()) << std::endl;
		// ["OK","23f8ced2dd2fe3e8f61e11b60d870bcbdbe0d922b509df7b940c2e8925855b97",true,""]
		// ["NOTICE","ERROR: bad msg: unparseable message"]
		auto msg = beast::buffers_to_string(buffer.data());
		// TODO
		return msg.find("OK") != msg.npos;
	}
	catch(const std::exception& e)
	{
		std::cerr << argv[0] << argv[1] << argv[2] << " Error: " << e.what() << std::endl;
		return false;;
	}

	return true;
}

}
