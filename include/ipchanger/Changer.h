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



inline void ChangeIP2(const std::string& ip, unsigned int port, const std::string& path)
{
    using clock = std::chrono::system_clock;
    using ms = std::chrono::milliseconds;
    const auto before = clock::now(); // Measure time


    namespace fs = boost::filesystem;

    auto constexpr TIBIA = (system::current_os == system::OS::Windows) ? "Tibia.exe" : "Tibia";
    auto unique_name = (system::current_os == system::OS::Windows) ? fs::unique_path(".%%%%_%%%%_%%%%_%%%%.exe") : fs::unique_path(".%%%%_%%%%_%%%%_%%%%"); // Generates a unqiue name starting with "." (hidden)
    auto fs_path = fs::path(path);

    std::cout << fs_path.string() << std::endl;

    if(!fs::exists(fs_path))
        return;

    auto full_path = fs::path(fs_path.string() + "/" + unique_name.string());
    auto tibia_path = fs::path(fs_path.string() + "/" + TIBIA);

    if(!fs::exists(tibia_path))
        return;

    std::cout << tibia_path.string() << std::endl;
    std::cout << unique_name.string() << std::endl;

    fs::ifstream tibia_binary(tibia_path, std::ios::binary);    // Create a FILE-pointer to the binary
    tibia_binary.seekg(0, fs::ifstream::end); // Seek to the end of the file
    std::size_t size = tibia_binary.tellg();  // Read the file size
    tibia_binary.seekg(0, fs::ifstream::beg); // Seek to the beginning of the file
    std::vector<char> buff(size);
    tibia_binary.read(buff.data(), size);     // Write the contents to "buff"
    tibia_binary.close();

    const std::string p1 = "login0"; // login0x.tibia.com
    const std::string p2 = "cipsoft."; // tibia0x.cipsoft.com

    std::vector<char> p1_v(p1.begin(), p1.end()); // First login pattern
    std::vector<char> p2_v(p2.begin(), p2.end()); // Second login pattern
    std::vector<system::AsciiNum> p3_v(300);      // RSA_KEY pattern
    std::vector<std::uintptr_t> offsets;

    // Match against the 3 patterns in 3 separate threads
    auto matcher = new system::StreamMatcher<char>(p1_v);
    auto matcher2 = new system::StreamMatcher<char>(p2_v);
    auto matcher3 = new system::StreamMatcher<system::AsciiNum>(p3_v);

    matcher->AddToQueue(buff);
    matcher2->AddToQueue(buff);
    matcher3->AddToQueue(buff);

    matcher->Stop();
    matcher2->Stop();
    matcher3->Stop();

    // The result is offsets in the buffer
    for(auto a : matcher->OffsetToWord())
        offsets.push_back(a);
    for(auto a : matcher2->OffsetToWord())
        offsets.push_back(a);

    // Replace Tibia's login IP's with the specified IP
    for(auto a : offsets)
        std::memcpy(&buff[a], ip.data(), ip.size() + 1);

    // Replace the RSA Address
    for(auto a : matcher3->OffsetToWord()) {
        std::memcpy(&buff[a], RSA_KEY.data(), RSA_KEY.size() + 1);
    }

    const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);
    std::cout << "Duration : " << duration.count() / 1000.0f << std::endl;

    fs::ofstream output(full_path, std::ios::binary); // Create a temporary file
    output.write(buff.data(), buff.size()); // Write the modified binary
    output.close();
    fs::permissions(full_path, fs::owner_all); // Allow all permissions to be able to execute it
    fs::current_path(fs_path); // Change the directory Tibia directory
    boost::process::system(full_path); // Execute the temporary file
    fs::remove(full_path); // Remove it immediately

}


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

    fs::ifstream tibia_binary(tibia_path, std::ios::binary); // Create a FILE-pointer to the binary
    tibia_binary.seekg(0, fs::ifstream::end); // Seek to the end of the file
    std::size_t size = tibia_binary.tellg();  // Read the file size
    tibia_binary.seekg(0, fs::ifstream::beg); // Seek to the beginning of the file
    std::vector<char> buff(size);
    tibia_binary.read(buff.data(), size);     // Write the contents to "buff"
    tibia_binary.close();

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

    fs::ofstream output(full_path, std::ios::binary); // Create a temporary file
    output.write(buff.data(), buff.size()); // Write the modified binary
    output.close();
    fs::permissions(full_path, fs::owner_all); // Allow all permissions to be able to execute it
    fs::current_path(fs_path); // Change the directory Tibia directory
    boost::process::system(full_path); // Execute the temporary file
    fs::remove(full_path); // Remove it immediately

}


} // namespace

#endif // CHANGER_H
