#ifndef CHANGER_H
#define CHANGER_H

#include "ipchanger/Process.h"
#include "ipchanger/Client.h"
#include "ipchanger/System.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <boost/process.hpp>

namespace ipchanger::changer {

// This function is deprectaed, only works on linux and pre 9.71
inline int ChangeIP_DEPRECATED(const std::string& ip, unsigned int port, unsigned int version)
{
    auto client = FindClient(version);
    if (!client.has_value())
		return 0; // Version not found

	std::string command_target = "Tibia";
    system::Process proc(command_target);

	if (!proc.GetPid())
		return 1; // Tibia not running

	if (!proc.Attach())
		return 2; // Administration rights required

    auto addr = client->rsa_address; // Holder for write address
    if (!proc.WriteToAddress<char>(addr, RSA_KEY, std::strlen(RSA_KEY) + 1))
		return 3; // Could not write RSA Key

	for (unsigned int i = 0; i < client->count; i++) {

		addr = client->write_address + i * client->step;
        if (!proc.WriteToAddress<char>(addr, ip.data(), ip.size()))
			return 4; // Could not write ip

		addr = client->write_address + i * client->step + client->port_offset;
        if (!proc.WriteToAddress<unsigned int>(addr, &port, 1))
			return 5; // Could not write port
	}

	return 6; // Success
}

inline void ChangeIP(const std::string& ip, unsigned int port, const std::string& path)
{
    namespace fs = boost::filesystem;

    auto constexpr TIBIA = (system::current_os == system::OS::Windows) ? "Tibia.exe" : "Tibia";
    auto unique_name = fs::unique_path(".%%%%_%%%%_%%%%_%%%%"); // Generates a unqiue name starting with "." (hidden)
    auto fs_path = fs::path(path);

    if(!fs::exists(fs_path))
        return;

    auto full_path = fs_path / unique_name;
    auto tibia_path = fs_path / TIBIA;

    if(!fs::exists(tibia_path))
        return;

    fs::ifstream tibia_binary(tibia_path);    // Create a FILE-pointer to the binary
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
        std::memcpy(&buff[a], ipchanger::RSA_KEY, std::strlen(ipchanger::RSA_KEY) + 1);
    }

    fs::ofstream output(full_path); // Create a temporary file
    output.write(buff.data(), buff.size()); // Write the modified binary
    output.close();
    fs::permissions(full_path, fs::owner_all); // Allow all permissions to be able to execute it
    fs::current_path(fs_path); // Change the directory Tibia directory
    boost::process::spawn(full_path); // Spawn a child process to open the temp file
    fs::remove(full_path); // Remove it immediately
}

} // namespace

#endif // CHANGER_H
