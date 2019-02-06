#pragma once
#ifndef PROCESS_H
#define PROCESS_H

namespace ipchanger::system {

	class Process {
	private:
		void* _handle;
		int _pid;
	public:
		Process(int pid);
		Process(const std::string& process_name);
		~Process();
		int FindPid(const std::string& process_name) const;
		int GetPid() const;
		int Attach(); // Usually requires administrator rights
		int Detach();
		void ReadMemory(void* addr, void* buff, std::size_t length);
		void WriteMemory(void* addr, std::size_t* buff, std::size_t length);

		template <class T>
		int WriteToAddress(std::uintptr_t addr, const T* buff, std::size_t length) const
		{
			WriteMemory(addr, reinterpret_cast<void*>(buff), length);
			return 1;
		}

		template <class T>
		int ReadFromAddress(std::uintptr_t addr, T* buff, std::size_t length) const
		{
			ReadMemory(addr, reinterpret_cast<void*>(buff), length);
			return 0;
		}

		template <class T>
		std::vector<T> ReadFromAddress(std::uintptr_t addr, std::size_t length) const
		{
			std::vector<T> buff(length);
			ReadMemory(reinterpret_cast<const void*>(addr), reinterpret_cast<void*>(buff.data()), length);
			return buff;
		}

		template <class T>
		void PrintMemory(std::uintptr_t addr, std::size_t size = 1)
		{
			auto buff = ReadFromAddress<T>(addr, size);
			std::stringstream out;

			for (T t : buff)
				out << t;
			out << std::endl;

			std::cout << out.str();
		}

	};

}

#endif // PROCESS_H