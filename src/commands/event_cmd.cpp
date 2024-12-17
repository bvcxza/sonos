#include "event_cmd.h"

#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <boost/algorithm/string/join.hpp>
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
	return R"(
		Create and send a event to nostr relays.
		Usage: echo "content" | sonos event <nsec> <kind> <tags> <relay_addresses ...>
	)";
}

bool event_cmd::execute(int argc, char* argv[])
{
	if (argc < 6) return false;

	net::io_context ioc;
	ssl::context ctx{ssl::context::tlsv12_client};
	ctx.set_default_verify_paths();
	tcp::resolver resolver{ioc};

	nostr nstr { argv[2] };
	uint16_t kind = std::atoi(argv[3]);
	std::string tags = argv[4];
	std::vector<std::string> lines;
	for (std::string line; std::getline(std::cin, line); lines.push_back(line));
	std::string content = boost::algorithm::join(lines, "\\n");
	std::string event = nstr.make_event(kind, content, tags);
	bool ok = false;
	for (int i = 5; i < argc; ++i)
	{
		std::string host_address = argv[i];
		auto&& [host, port] = split_pair(host_address, ':');
		try
		{
			websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
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

			std::cout << "handshaking to " << host_address << std::endl;
			ws.handshake(host_address, "/");
			ws.write(net::buffer(event));
			std::cout << "write: " << event << std::endl;
			beast::flat_buffer buffer;
			ws.read(buffer);
			ws.close(websocket::close_code::normal);
			std::cout << "read [" << host_address << "]: " << beast::make_printable(buffer.data()) << std::endl;
			// ["OK","23f8ced2dd2fe3e8f61e11b60d870bcbdbe0d922b509df7b940c2e8925855b97",true,""]
			// ["NOTICE","ERROR: bad msg: unparseable message"]
			auto msg = beast::buffers_to_string(buffer.data());
			// TODO
			ok = msg.find("OK") != msg.npos;
		}
		catch(const std::exception& e)
		{
			std::cerr << "Error [" << host_address << "]: " << e.what() << std::endl;
		}
	}
	if (!ok) throw error("Sending event fail");

	return true;
}

}
