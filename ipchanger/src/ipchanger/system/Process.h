#pragma once
#ifndef PROCESS_H
#define PROCESS_H

#ifdef _WIN32
#include "ipchanger/system/platform/windows/Process_Windows.h"
#else
#include "ipchanger/system/platform/linux/Process_Unix.h"
#endif
#include "ipchanger/system/System.h"

namespace ipchanger::system {

	class Process : public ProcessBase {
	public:
		Process(const std::string& process_name);
		Process(int pid);

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

} // namespace

#endif // PROCESS_H
