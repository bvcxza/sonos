#pragma once

#include <cstdlib>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <socks5.hpp>

#include "utils.h"

namespace sonos
{

template <class Socket, class Resolver>
void connect(Socket& socket, Resolver& resolver, const std::string& host, const std::string& port)
{
	auto&& results = resolver.resolve(host, port);
	if (const char* socks_endpoint = std::getenv("SONOS_SOCKS_ENDPOINT"))
	{
		auto&& [proxy_host, proxy_port] = split_pair(socks_endpoint, ':');
		auto&& proxy_result = resolver.resolve(proxy_host, proxy_port).begin();
		socks5::proxy_connect(socket, results.begin()->endpoint(), proxy_result->endpoint());
	}
	else
	{
		boost::asio::connect(socket, results);
	}
}

}
