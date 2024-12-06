#include "req_cmd.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>

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
		Send a REQ message to nostr relays and execute a command for each readed event.
		Usage: sonos req <filters> <command> <relay_addresses ...>
	)";
}

bool req_cmd::execute(int argc, char* argv[])
{
	if (argc < 5) return false;

	try
	{
		using namespace boost::json;
		net::io_context ioc;
		ssl::context ctx{ssl::context::tlsv12_client};
		ctx.set_default_verify_paths();
		tcp::resolver resolver{ioc};
		static websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
		std::string host_address = argv[4];
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
		std::string command = argv[3];
		std::string req_msg = R"(["REQ","${subscription_id}",${filters}])";
		assert(replaceAll(req_msg, {{"${subscription_id}",subscription_id},{"${filters}",filters}}));
		std::cout << "req_msg: " << req_msg << std::endl;
		std::cout << "handshaking to " << host_address << std::endl;
		ws.handshake(host_address, "/");
		ws.write(net::buffer(req_msg));
		auto in_file_name = "sonos" + subscription_id;
		std::filesystem::path in_file_path = std::filesystem::temp_directory_path() / in_file_name;
		std::string fullCmd = "${command} ${in_file_path}";
		assert(replaceAll(fullCmd, {{"${command}",command},{"${in_file_path}",in_file_path.string()}}));
		for (;ws.is_open();)
		{
			beast::flat_buffer buffer;
			ws.read(buffer);
			value vjson = parse(beast::buffers_to_string(buffer.data()));
			auto&& evt_array = vjson.as_array();
			if (evt_array.size() == 3 && value_to<std::string>(evt_array[0]) == "EVENT")
			{
				std::ofstream out(in_file_path);
				out << serialize(evt_array[2].as_object());
				out.close();
				std::cout << "command: " << fullCmd << std::endl;
				std::system(fullCmd.c_str());
			}
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
