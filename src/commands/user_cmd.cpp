#include "user_cmd.h"

#include <iostream>
#include <map>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>

#include "../pubkey.h"
#include "../utils.h"
#include "../net_utils.h"

namespace sonos
{

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

std::string user_cmd::help() const
{
	return R"(
		Show user metadata from hexadecimal/bech32 public key.
		Usage: sonos user <public_key> <relay_addresses ...>
	)";
}

bool user_cmd::execute(int argc, char* argv[])
{
	if (argc < 4) return false;

	using namespace boost::json;
	net::io_context ioc;
	ssl::context ctx{ssl::context::tlsv12_client};
	ctx.set_default_verify_paths();
	tcp::resolver resolver{ioc};

	std::string req_msg = R"(["REQ","${subscription_id}",{"kinds":[0],"authors":["${pub_key}"],"limit":1}])";
	{
		std::string subscription_id = sha256(random(8));
		pubkey pub_key { argv[2] };
		assert(replaceAll(req_msg, {{"${subscription_id}",subscription_id},{"${pub_key}",pub_key.to_hex()}}));
	}
	bool ok = true;
	for (int i = 3; i < argc; ++i)
	{
		std::string host_address = argv[i];
		auto&& [host, port] = split_pair(host_address, ':');

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

			ws.handshake(host_address, "/");
			ws.write(net::buffer(req_msg));
			beast::flat_buffer buffer;
			ws.read(buffer);
			try{
			ws.close(websocket::close_code::normal);
			}catch(...){ /*socks5 throws "Transport endpoint is not connected"*/ }
			value vjson = parse(beast::buffers_to_string(buffer.data()));
			auto&& evt_array = vjson.as_array();
			if (evt_array.size() == 3 && value_to<std::string>(evt_array[0]) == "EVENT")
			{
				std::cout << serialize(evt_array[2].as_object()) << std::endl;
				return true;
			}
			else if (evt_array.size() == 2 && value_to<std::string>(evt_array[0]) == "EOSE")
			{
				continue;
			}
		}
		catch (const std::exception& e)
		{
			ok = false;
			std::cerr << "Error [" << host_address << "]: " << e.what() << std::endl;
			continue;
		}
	}

	if (!ok) throw error("REQ user fail");

	return true;
}

}
