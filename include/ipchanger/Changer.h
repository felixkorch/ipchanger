#ifndef CHANGER_H
#define CHANGER_H

#include <boost/filesystem.hpp>
#include "ipchanger/System.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <functional>

namespace ipchanger {

static const std::string RSA_KEY =
        "10912013296739942927886096050899554152823750290279"
        "81291234687579372662914925764463307396960011106039"
        "07230888610072655818825358503429057592827629436413"
        "10856602909362821263595383668656267584972062078627"
        "94310902180176810615217550567108238764764442605581"
        "47179707119674283982419152118103759076030616683978566631413";

static const std::string p1{ "login0" }; // login0x.tibia.com
static const std::string p2 { "cipsoft." }; // tibia0x.cipsoft.com
static const std::string rsa1{ "1321277432058722840622950990822933849527763264961655079678763" }; // First try to find this RSA
static const std::string rsa2{ "124710459426827943004376449897985582167801707960697037164044904" }; // Fallback RSA (pre 8.61)

namespace fs = boost::filesystem;

inline bool SearchAndReplace(std::string& in, const std::string& pattern, const std::string& replace)
{
    auto search = std::search(in.begin(), in.end(), std::boyer_moore_searcher(pattern.begin(), pattern.end()));
    if(search == in.end())
        return false;
    while(search != in.end()) {
        std::copy(replace.c_str(), replace.c_str() + replace.size() + 1, search);
        search = std::search(search, in.end(), std::boyer_moore_searcher(pattern.begin(), pattern.end()));
    }
    return true;
}

inline std::string ChangeIP(const std::string& ip, unsigned int port, const fs::path& in)
{
    std::string buff{ system::ReadFile<std::string>(in) };

    SearchAndReplace(buff, p1, ip);
    SearchAndReplace(buff, p2, ip);
    if(!SearchAndReplace(buff, rsa1, RSA_KEY))
        SearchAndReplace(buff, rsa2, RSA_KEY);

    return buff;
}

} // namespace

#endif // CHANGER_H
