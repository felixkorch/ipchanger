#pragma once
#ifndef PROCESS_H
#define PROCESS_H

namespace ipchanger::system {

	class Process {
	private:
		void* _handle;
		int _pid;
	private:
		void ReadMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length);
		void WriteMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length);
	public:
		Process(int pid);
		Process(const std::string& process_name);
		~Process();
		int FindPid(const std::string& process_name) const;
		int GetPid() const;
		int Attach(); // Usually requires administrator rights
		int Detach();

		int WriteToAddress(std::uintptr_t addr, std::size_t* buff, std::size_t length)
		{
			WriteMemory(addr, buff, length);
			return 1;
		}

		int ReadFromAddress(std::uintptr_t addr, std::size_t* buff, std::size_t length)
		{
			ReadMemory(addr, buff, length);
			return 0;
		}

		std::vector<char> ReadFromAddress(std::uintptr_t addr, std::size_t length)
		{
			std::vector<char> buff(length);
			ReadMemory(addr, (std::size_t*)buff.data(), length);
			return buff;
		}

		/* TODO
		template <class T>
		void PrintMemory(std::uintptr_t addr, std::size_t size = 1)
		{
			auto buff = ReadFromAddress<T>(addr, size);
			std::stringstream out;

			for (T t : buff)
				out << t;
			out << std::endl;

			std::cout << out.str();
		} */

	};

}

#endif // PROCESS_H