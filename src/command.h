#pragma once

#include <exception>
#include <string>

namespace sonos
{

class command
{
public:
	class error : public std::exception
	{
	public:
		error(const std::string& what_arg) : m_what_arg(what_arg) {}
		const char* what() const noexcept override { return m_what_arg.c_str(); }
	private:
		std::string m_what_arg;
	};

	virtual std::string help() const = 0;
	virtual bool execute(int argc, char* argv[]) = 0;
};

}
