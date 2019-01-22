#ifndef CHANGER_H
#define CHANGER_H

#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include "ipchanger/System.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <algorithm>

namespace ipchanger::changer {

const std::string RSA_KEY =
        "10912013296739942927886096050899554152823750290279"
        "81291234687579372662914925764463307396960011106039"
        "07230888610072655818825358503429057592827629436413"
        "10856602909362821263595383668656267584972062078627"
        "94310902180176810615217550567108238764764442605581"
        "47179707119674283982419152118103759076030616683978566631413";

inline void ChangeIP(const std::string& ip, unsigned int port, const std::string& path)
{
    namespace fs = boost::filesystem;

    auto constexpr TIBIA = (system::current_os == system::OS::Windows) ? "Tibia.exe" : "Tibia";
    auto constexpr RAND = (system::current_os == system::OS::Windows) ? ".%%%%_%%%%_%%%%_%%%%.exe" : ".%%%%_%%%%_%%%%_%%%%";
    auto unique_name = fs::unique_path(RAND); // Generates a unqiue name
    auto fs_path = fs::path(path);

    auto full_path = fs_path / unique_name;
    auto tibia_path = fs_path / TIBIA;

    if(!fs::exists(fs_path) || !fs::exists(tibia_path))
        return;

    auto buff = ReadFile(tibia_path);

    const std::string p1 = "login0"; // login0x.tibia.com
    const std::string p2 = "cipsoft."; // tibia0x.cipsoft.com

    auto sear = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    while(sear != buff.end()) {
        std::copy(ip.data(), &ip.back() + 1, sear);
        sear = std::search(sear, buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    }

    auto sear2 = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    while(sear2 != buff.end()) {
        std::copy(ip.data(), &ip.back() + 1, sear2);
        sear2 = std::search(sear2, buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    }

    const std::string rsa1 = "1321277432058722840622950990822933849527763264961655079678763";
    const std::string rsa2 = "124710459426827943004376449897985582167801707960697037164044904"; // TODO: Fix Regex search
    auto sear3 = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(rsa1.begin(), rsa1.end()));
    if(sear3 != buff.end()) { // First try to find this RSA
        std::copy(std::begin(RSA_KEY), std::end(RSA_KEY), sear3);
    }else { // Fallback RSA
        auto s = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(rsa2.begin(), rsa2.end()));
        if(s != buff.end())
            std::copy(std::begin(RSA_KEY), std::end(RSA_KEY), s);
    }

    WriteFile(full_path, buff);
    fs::permissions(full_path, fs::owner_all); // Allow all permissions to be able to execute it
    fs::current_path(fs_path); // Change the directory Tibia directory
    boost::process::system(full_path); // Execute the temporary file
    fs::remove(full_path); // Remove it immediately

}


} // namespace

#endif // CHANGER_H
