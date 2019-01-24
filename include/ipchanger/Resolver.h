#ifndef RESOLVER_H
#define RESOLVER_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <optional>

namespace ipchanger::system {

#ifdef _WIN32
#include "winsock.h"
#else
#include <netdb.h>
#include <arpa/inet.h>
#endif

static void initialise(void)
{
#ifdef _WIN32
    WSADATA data;
    if (WSAStartup (MAKEWORD(1, 1), &data) != 0)
    {
        fputs ("Could not initialise Winsock.\n", stderr);
        exit (1);
    }
#endif
}

static void uninitialise (void)
{
#ifdef _WIN32
    WSACleanup ();
#endif
}

std::optional<std::string> Resolve(const std::string& hostname)
{
    struct hostent* he;
    initialise();
    he = gethostbyname(hostname.c_str());
    uninitialise();
    const char* adr = inet_ntoa(*((struct in_addr*) he->h_addr_list[0]));
    auto ip = std::make_optional(std::string(adr));
    return ip;
}

} // namespace

#endif // RESOLVER_H
