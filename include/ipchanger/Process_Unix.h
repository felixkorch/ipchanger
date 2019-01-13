#ifndef UNIXPROCESS_HPP
#define UNIXPROCESS_HPP

#include "ipchanger/Client.h"
#include "boost/process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <iostream>

namespace ipchanger::system {

class ProcessBase {
private:
    int _pid;
public:
    ProcessBase(int pid)
        : _pid(pid)
    {}

    ProcessBase(const std::string& process_name)
        : _pid(FindPid(process_name))
    {}

    ~ProcessBase()
    {
        Detach();
    }

    int GetPid() const
    {
        return _pid;
    }

    int FindPid(const std::string& process_name) const
    {
        namespace bp = boost::process;
        bp::ipstream command_output;
        int run_command = !bp::system("pgrep " + process_name, bp::std_out > command_output);

        std::vector<std::string> data;
        std::string line;

        while (std::getline(command_output, line) && !line.empty())
            data.push_back(line);

        return data.size() > 0 ? std::stoi(data[0]) : 0;
    }

    int Attach() const // Tries to attach to a process, usually requires root
    {
        if (ptrace(PTRACE_ATTACH, _pid, 0, 0) != 0)
            return 0;
        wait(0);
        return 1;
    }

    int Detach() const
    {
        if (ptrace(PTRACE_DETACH, _pid, 0, 0) != 0)
            return 0;
        return 1;
    }

    int WriteToAddress(std::uintptr_t addr, const char* buff, std::size_t length) const
    {
        for (std::size_t i = 0; i < length; i++) {
            ptrace(PTRACE_POKEDATA, _pid, addr + i, buff[i]);
        }
        return 1;
    }

    int ReadFromAddress(std::uintptr_t addr, char* buff, std::size_t length) const
    {
        for (std::size_t i = 0; i < length; i++) {
            buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
        }
        return 1;
    }

    std::vector<char> ReadFromAddress(std::uintptr_t addr, std::size_t length) const
    {
        std::vector<char> buff(length);
        for (std::size_t i = 0; i < length; i++) {
            buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
        }
        return buff;
    }

};

} // end ipchanger namespace

#endif // UNIXPROCESS_HPP
