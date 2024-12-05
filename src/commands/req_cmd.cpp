#include "req_cmd.h"

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include "../utils.h"

namespace sonos
{

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string req_cmd::help() const
{
	return R"(
		Send a REQ message to nostr relays and listener for events.
		Usage: sonos req <filters> <relay_addresses ...>
	)";
}

bool req_cmd::execute(int argc, char* argv[])
{
	if (argc < 4) return false;

	try
	{
		net::io_context ioc;
		ssl::context ctx{ssl::context::tlsv12_client};
		ctx.set_default_verify_paths();
		tcp::resolver resolver{ioc};
		static websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
		std::string host_address = argv[3];
		auto&& [host, port] = split_pair(host_address, ':');
		auto const results = resolver.resolve(host, port);
		net::connect(beast::get_lowest_layer(ws), results);

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

		// ["REQ", <subscription_id>, <filters1>, <filters2>, ...]
		static std::string subscription_id = sha256(random(8));
		std::string filters = argv[2];
		std::string req_msg = R"(["REQ","${subscription_id}",${filters}])";
		bool result = replaceAll(req_msg, {{"${subscription_id}",subscription_id},{"${filters}",filters}});
		assert(result);
		std::cout << "req_msg: " << req_msg << std::endl;
		std::cout << "handshaking to " << host_address << std::endl;
		ws.handshake(host_address, "/");
		auto handler = [](int signal){
			std::string close_msg = R"(["CLOSE", "${subscription_id}"])";
			assert(replaceAll(close_msg, {{"${subscription_id}",subscription_id}}));
			ws.write(net::buffer(close_msg));
			ws.close(websocket::close_code::normal);
			std::cout << "closed: " << close_msg << " by signal " << signal << std::endl;
		};
		std::signal(SIGTERM,handler);
		std::signal(SIGINT,handler);
		ws.write(net::buffer(req_msg));
		for (;ws.is_open();)
		{
			try
			{
				beast::flat_buffer buffer;
				ws.read(buffer);
				std::cout << "read: " << beast::make_printable(buffer.data()) << std::endl;
			}
			catch (const boost::system::system_error&) {}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << argv[0] << ' ' << argv[1] << ' ' << argv[2] << " Error: " << e.what() << std::endl;
		return false;;
	}

	return true;
}

}
