#ifndef CHANGER_H
#define CHANGER_H

#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include "ipchanger/System.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <regex>

namespace ipchanger::changer {

const std::string RSA_KEY =
        "10912013296739942927886096050899554152823750290279"
        "81291234687579372662914925764463307396960011106039"
        "07230888610072655818825358503429057592827629436413"
        "10856602909362821263595383668656267584972062078627"
        "94310902180176810615217550567108238764764442605581"
        "47179707119674283982419152118103759076030616683978566631413";

namespace fs = boost::filesystem;
inline void ChangeIP(const std::string& ip, unsigned int port, const fs::path& in, const fs::path& out)
{
    auto buff = system::ReadFileString(in);

    const std::string p1 = "login0"; // login0x.tibia.com
    const std::string p2 = "cipsoft."; // tibia0x.cipsoft.com

    auto search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    while(search != buff.end()) {
        std::copy(ip.c_str(), ip.c_str() + ip.size() + 1, search);
        search = std::search(search, buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    }

    search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    while(search != buff.end()) {
        std::copy(ip.c_str(), ip.c_str() + ip.size() + 1, search);
        search = std::search(search, buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    }

    std::regex rsa_regex("[0-9]{300}");
    std::smatch sm;
    if(std::regex_search(buff, sm, rsa_regex))
        std::copy(std::begin(RSA_KEY), std::end(RSA_KEY), &buff[sm.position()]);

    system::WriteFileString(out, buff);
    fs::permissions(out, fs::owner_all); // Allow all permissions to be able to execute it
}

void LaunchTemporary(const fs::path& path)
{
    if(!fs::exists(path))
        return;
    fs::current_path(path.parent_path()); // Change to the Tibia directory
    boost::process::system(path);         // Execute the temporary file
    fs::remove(path);
}


} // namespace

#endif // CHANGER_H
