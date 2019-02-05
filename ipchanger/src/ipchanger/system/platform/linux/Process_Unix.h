#pragma once
#ifndef UNIXPROCESS_HPP
#define UNIXPROCESS_HPP

#include "pch.h"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>

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
			std::string command = "pgrep " + process_name;
			char buf[20];

			FILE *stream = popen(command.c_str(), "r");
			fgets(buf, 20, stream);
			int pid = std::atoi(buf);
			pclose(stream);

			return pid;
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

		template <class T>
		int WriteToAddress(std::uintptr_t addr, const T* buff, std::size_t length) const
		{
			for (std::size_t i = 0; i < length; i++) {
				ptrace(PTRACE_POKEDATA, _pid, addr + i, buff[i]);
			}
			return 1;
		}

		template <class T>
		int ReadFromAddress(std::uintptr_t addr, T* buff, std::size_t length) const
		{
			for (std::size_t i = 0; i < length; i++) {
				buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
			}
			return 1;
		}

		template <class T>
		std::vector<T> ReadFromAddress(std::uintptr_t addr, std::size_t length) const
		{
			std::vector<T> buff(length);
			for (std::size_t i = 0; i < length; i++) {
				buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
			}
			return buff;
		}

};

} // namespace

#endif // UNIXPROCESS_HPP
