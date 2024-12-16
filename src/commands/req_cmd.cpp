#include "req_cmd.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

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
		Send a REQ message to nostr relays and execute a external command for each received event.
		The program called by command receives the event JSON file path as the first parameter ($1 in bash).
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
		tcp::resolver resolver{ioc};
		ssl::context ctx{ssl::context::tlsv12_client};
		ctx.set_default_verify_paths();
		std::string subscription_id = sha256(random(8));
		auto in_file_name = "sonos" + subscription_id;
		const std::filesystem::path in_file_path = std::filesystem::temp_directory_path() / in_file_name;
		// ["REQ", <subscription_id>, <filters1>, <filters2>, ...]
		std::string req_msg = R"(["REQ","${subscription_id}",${filters}])";
		std::string fullCmd = "${command} ${in_file_path}";
		{
			std::string filters = argv[2];
			std::string command = argv[3];
			assert(replaceAll(req_msg, {{"${subscription_id}",subscription_id},{"${filters}",filters}}));
			assert(replaceAll(fullCmd, {{"${command}",command},{"${in_file_path}",in_file_path.string()}}));
		}
		std::mutex mutex;
		std::unordered_set<std::string> evt_id_set; // TODO limit
		auto handle_event = [&in_file_path,&fullCmd,&mutex,&evt_id_set](websocket::stream<ssl::stream<tcp::socket>>&& ws){
			for (;ws.is_open();)
			{
				beast::flat_buffer buffer;
				ws.read(buffer);
				value vjson = parse(beast::buffers_to_string(buffer.data()));
				auto&& evt_array = vjson.as_array();
				if (evt_array.size() == 3 && value_to<std::string>(evt_array[0]) == "EVENT")
				{
					std::scoped_lock lock(mutex);
					auto&& evt_obj = evt_array[2].as_object();
					auto&& evt_id = value_to<std::string>(evt_obj["id"]);
					if (evt_id_set.contains(evt_id)) continue;

					std::ofstream out(in_file_path);
					out << serialize(evt_obj);
					out.close();
					std::cout << "command: " << fullCmd << std::endl;
					std::system(fullCmd.c_str());

					evt_id_set.insert(evt_id);
				}
			}
		};
		std::vector<std::thread> threads;
		threads.reserve(argc - 4 - 1);
		for (int i = 4; i < argc; ++i)
		{
			websocket::stream<ssl::stream<tcp::socket>> ws{ioc, ctx};
			std::string host_address = argv[i];
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

			std::cout << "req_msg: " << req_msg << std::endl;
			std::cout << "handshaking to " << host_address << std::endl;
			ws.handshake(host_address, "/");
			ws.write(net::buffer(req_msg));

			if (i < (argc - 1))
				threads.emplace_back(handle_event, std::move(ws));
			else // last job in current thread
				handle_event(std::move(ws));
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << argv[0] << ' ' << argv[1] << ' ' << argv[2] << ' ' << argv[3] << ' ' << argv[4] << " Error: " << e.what() << std::endl;
		return false;;
	}

	return true;
}

}
