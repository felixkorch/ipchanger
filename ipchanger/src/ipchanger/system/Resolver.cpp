#include "pch.h"
#include "ipchanger/system/Resolver.h"

static void Initialize()
{
#ifdef _WIN32
	WSADATA data;
	if (WSAStartup(MAKEWORD(1, 1), &data) != 0)
	{
		fputs("Could not initialise Winsock.\n", stderr);
		exit(1);
	}
#endif
}

static void UnInitialize()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

namespace ipchanger::system {

	std::optional<std::string> Resolve(const std::string& hostname)
	{
		struct hostent* he;
		Initialize();
		he = gethostbyname(hostname.c_str());
		UnInitialize();

		if (he == nullptr)
			return std::optional<std::string>{};

		const char* adr = inet_ntoa(*((struct in_addr*) he->h_addr_list[0]));
		auto ip = std::make_optional(std::string(adr));
		return ip;
	}

}