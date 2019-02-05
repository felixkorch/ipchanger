#include "pch.h"
#include "ipchanger/system/Process.h"

namespace ipchanger::system {

	Process::Process(const std::string& process_name)
		: ProcessBase(process_name) {}

	Process::Process(int pid)
		: ProcessBase(pid) {}

} // namespace
