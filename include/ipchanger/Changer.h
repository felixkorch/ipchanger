#ifndef CHANGER_H
#define CHANGER_H

#include "ipchanger/Process.h"
#include "ipchanger/Client.h"
#include <string>

namespace ipchanger::changer {

inline int ChangeIP(const std::string& ip, unsigned int port, unsigned int version)
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

} // end ipchanger namespace

#endif // CHANGER_H
