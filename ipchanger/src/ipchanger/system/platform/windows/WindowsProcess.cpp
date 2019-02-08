#include "pch.h"
#include "Process.h"

namespace ipchanger::system {

	Process::Process(int pid)
		: _handle(nullptr), _pid(pid) {}

	Process::Process(const std::string& process_name)
		: _pid(FindPid(process_name + ".exe")) {}


	Process::~Process()
	{
		Detach();
	}

	int Process::FindPid(const std::string& process_name) const
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		std::wstring widestr = std::wstring(process_name.begin(), process_name.end());
		const wchar_t* process = widestr.c_str();

		if (Process32First(snapshot, &entry) == TRUE)
		{
			while (Process32Next(snapshot, &entry) == TRUE)
			{
				if (lstrcmpi(entry.szExeFile, widestr.c_str()) == 0)
				{
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
					int actualProcId = GetProcessId(hProcess);
					CloseHandle(hProcess);
					return actualProcId;
				}
			}
		}

		return 0;

		CloseHandle(snapshot);
	}

	int Process::GetPid() const
	{
		return _pid;
	}

	int Process::Attach() // Tries to attach to a process, usually requires administrator rights
	{
		DWORD access =
			PROCESS_VM_READ |
			PROCESS_QUERY_INFORMATION |
			PROCESS_VM_WRITE |
			PROCESS_VM_OPERATION;
		_handle = OpenProcess(access, FALSE, _pid);
		if (!_handle)
			return 0;
		return 1;
	}

	int Process::Detach()
	{
		CloseHandle(_handle);
		return 1;
	}

	void Process::WriteMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length)
	{
		if (!_handle) {
			std::cout << "Proccess must be attached." << std::endl;
			return;
		}

		SIZE_T written_bytes;
		WriteProcessMemory(_handle, reinterpret_cast<void*>(addr), reinterpret_cast<void*>(buff), length, &written_bytes);
	}

	void Process::ReadMemory(std::uintptr_t addr, std::size_t* buff, std::size_t length)
	{
		if (!_handle) {
			std::cout << "Proccess must be attached." << std::endl;
			return;
		}
		SIZE_T read_bytes;
		ReadProcessMemory(_handle, reinterpret_cast<void*>(addr), reinterpret_cast<void*>(buff), length, &read_bytes);
	}

}