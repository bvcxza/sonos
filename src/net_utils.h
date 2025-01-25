#pragma once

#include <cstdlib>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <socks5.hpp>

#include "utils.h"

namespace sonos
{

template <class Socket, class Resolver, class QueryTarget>
void connect(Socket& socket, Resolver& resolver, const QueryTarget& query)
{
	auto const results = resolver.resolve(query);
	if (const char* socks_endpoint = std::getenv("SONOS_SOCKS_ENDPOINT"))
	{
		auto&& [host, port] = split_pair(socks_endpoint, ':');
		auto const proxy_results = resolver.resolve(host, port);
		socks5::proxy_connect(socket, results->endpoint(), proxy_results->endpoint());
	}
	else
	{
		boost::asio::connect(socket, results);
	}
}

}
