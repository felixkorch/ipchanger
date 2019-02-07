#pragma once
#ifndef UNIXPROCESS_HPP
#define UNIXPROCESS_HPP

#include "pch.h"
#include "Process.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>

namespace ipchanger::system {

	Process::Process(int pid)
		: _pid(pid), _handle(nullptr) {}

	Process::Process(const std::string& process_name)
		: _pid(FindPid(process_name)) {}

	Process::~Process()
	{
		Detach();
	}

	int Process::GetPid() const
	{
		return _pid;
	}

	int Process::FindPid(const std::string& process_name) const
	{
		std::string command = "pgrep " + process_name;
		char buf[20];

		FILE *stream = popen(command.c_str(), "r");
		fgets(buf, 20, stream);
		int pid = std::atoi(buf);
		pclose(stream);

		return pid;
	}

	int Process::Attach()
	{
		if (ptrace(PTRACE_ATTACH, _pid, 0, 0) != 0)
			return 0;
		wait(0);
		return 1;
	}


	int Process::Detach()
	{
		if (ptrace(PTRACE_DETACH, _pid, 0, 0) != 0)
			return 0;
		return 1;
	}

	void Process::WriteMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length)
	{
		for (std::size_t i = 0; i < length; i++) {
			ptrace(PTRACE_POKEDATA, _pid, addr + i, buff[i]);
		}
	}

	void Process::ReadMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length)
	{
		for (std::size_t i = 0; i < length; i++) {
			buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
		}
	}

} // namespace

#endif // UNIXPROCESS_HPP
