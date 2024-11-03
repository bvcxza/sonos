
#include <cstdlib>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

#include <bech32.h>
#include <secp256k1.h>

#include "../examples/examples_util.h"

#include "key.h"
#include "commands/convert_cmd.h"
#include "commands/keypair_cmd.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace base64 = beast::detail::base64;         // from <boost/beast/core/detail/base64.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	// Check command line arguments.
	if (argc < 2)
	{
		std::cerr << "EXIT_FAILURE\n";
		return EXIT_FAILURE;
	}

	try
	{
		std::map<std::string, std::shared_ptr<sonos::command>> commands = {
			{"convert", std::make_shared<sonos::convert_cmd>()},
			{"keypair", std::make_shared<sonos::keypair_cmd>()}//,
		};

		auto cmd = commands[argv[1]];
		if (!cmd)
		{
			std::cerr << argv[0] << " Error: " << "TODO help" << std::endl;
			return EXIT_FAILURE;
		}
		try
		{
			if (!cmd->execute(argc, argv))
			{
				std::cerr << argv[0] << " Error\n" << cmd->help() << std::endl;
				return EXIT_FAILURE;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << argv[0] << " Error: " << e.what() << '\n' << cmd->help() << std::endl;
			return EXIT_FAILURE;
		}

		// TODO
		return EXIT_SUCCESS;

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

		auto deleter = [](secp256k1_context* ctx) { secp256k1_context_destroy(ctx); };
		std::unique_ptr<secp256k1_context, decltype(deleter)> secp256k1_ctx { secp256k1_context_create(SECP256K1_CONTEXT_NONE) };
		//std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> secp256k1_ctx ( secp256k1_context_create(SECP256K1_CONTEXT_NONE), &secp256k1_context_destroy );
		unsigned char randomize[32];
		if (!fill_random(randomize, sizeof(randomize)))
		{
			std::cerr << "Failed to generate randomness" << std::endl;
			return EXIT_FAILURE;
		}
		{
			auto ret = secp256k1_context_randomize(secp256k1_ctx.get(), randomize);
			assert(ret);
		}

		//std::string npub1 = "bc1qw508d6qejxtdg4y5r3zarvary0c5xw7kv8f3t4";
		//std::string npub1 = "a12uel5l";

		std::string npub1 = "npub1qqqq2trmvgcvvnk5rk9tttpj4kztyfdjp72t33lyqygnea5cun6swettq0";
		// 0000052c7b6230c64ed41d8ab5ac32ad84b225b20f94b8c7e401113cf698e4f5
		//std::string npub1 = "npub172qu2hx845rew3t83jrsmpqvq8zdlap2ta2nfdpe9ecphrsh670sjlz066";
		// f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f
		//std::string npub1 = "npub10elfcs4fr0l0r8af98jlmgdh9c8tcxjvz9qkw038js35mp4dma8qzvjptg";
		// 7e7e9c42a91bfef19fa929e5fda1b72e0ebc1a4c1141673e2794234d86addf4e

		sonos::key key = npub1;
		std::cout << "npub1: " << npub1 << " hex: " << key.to_hex() << " size=" << key.size() << std::endl;



		std::string text = R"(["EVENT",{"kind":1,"content":"hi nostr!!","tags":[],"created_at":1729681500,"pubkey":"f281c55cc7ad079745678c870d840c01c4dff42a5f5534b4392e701b8e17d79f","id":"3867e83395d0885f6898111d595fa9fff1f20352deea4f1994fed0ec1578e5c1","sig":"febf6bd1ea609676bfcdb7fd16e44606316c75cd7865c086c849799240053d038b73e0fba5169e365e5f564815e03bb9e18d36fa62bd793750a6b6945352ca7d"}])";
		host += ':' + std::to_string(ep.port());
		std::cout << "handshaking to " << host << std::endl;
		ws.handshake(host, "/");
		ws.write(net::buffer(text));
		std::cout << "write: " << text << std::endl;
		beast::flat_buffer buffer;
		ws.read(buffer);
		ws.close(websocket::close_code::normal);
		std::cout << "read: " << beast::make_printable(buffer.data()) << std::endl;
	}
	catch(std::exception const& e)
	{
		std::cerr << argv[0] << " Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << argv[0] << " EXIT_SUCCESS\n";
	return EXIT_SUCCESS;
}
