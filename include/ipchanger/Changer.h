#ifndef CHANGER_H
#define CHANGER_H

#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include "ipchanger/System.h"
#include <iostream>
#include <string>
#include <algorithm>

namespace ipchanger::changer {

static const std::string RSA_KEY =
        "10912013296739942927886096050899554152823750290279"
        "81291234687579372662914925764463307396960011106039"
        "07230888610072655818825358503429057592827629436413"
        "10856602909362821263595383668656267584972062078627"
        "94310902180176810615217550567108238764764442605581"
        "47179707119674283982419152118103759076030616683978566631413";

namespace fs = boost::filesystem;

inline void ChangeIP(const std::string& ip, unsigned int port, const fs::path& in, const fs::path& out)
{
    /*
        using clock = std::chrono::system_clock;
        using ms = std::chrono::milliseconds;
        const auto before = clock::now(); // Measure time
    */

    std::string buff = system::ReadFileString(in);

    const std::string p1 = "login0"; // login0x.tibia.com
    const std::string p2 = "cipsoft."; // tibia0x.cipsoft.com
    const std::string rsa1 = "1321277432058722840622950990822933849527763264961655079678763"; // First try to find this RSA
    const std::string rsa2 = "124710459426827943004376449897985582167801707960697037164044904"; // Fallback RSA (pre 8.61)

    // First search
    auto search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    while(search != buff.end()) {
        std::copy(ip.c_str(), ip.c_str() + ip.size() + 1, search);
        search = std::search(search, buff.end(), std::boyer_moore_searcher(p1.begin(), p1.end()));
    }

    // Second search
    search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    while(search != buff.end()) {
        std::copy(ip.c_str(), ip.c_str() + ip.size() + 1, search);
        search = std::search(search, buff.end(), std::boyer_moore_searcher(p2.begin(), p2.end()));
    }

    // Third search
    search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(rsa1.begin(), rsa1.end()));
    if(search != buff.end()) {
        std::copy(std::begin(RSA_KEY), std::end(RSA_KEY), search);
    }
    else {
        search = std::search(buff.begin(), buff.end(), std::boyer_moore_searcher(rsa2.begin(), rsa2.end()));
        if(search != buff.end())
            std::copy(std::begin(RSA_KEY), std::end(RSA_KEY), search);
    }

    system::WriteFileString(out, buff);
    fs::permissions(out, fs::owner_all); // Allow all permissions to be able to execute it

    /*
        const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);
        std::cout << "Duration : " << duration.count() / 1000.0f << std::endl;
    */
}

inline void LaunchTemporary(const fs::path& path)
{
    if(!fs::exists(path))
        return;

#ifdef _WIN32
    SetFileAttributes((LPCSTR)path.string().c_str(), FILE_ATTRIBUTE_HIDDEN); // Hide the file on Windows
#endif
    fs::current_path(path.parent_path()); // Change to the Tibia directory
    boost::process::system(path);         // Execute the temporary file
    fs::remove(path);
}


} // namespace

#endif // CHANGER_H
