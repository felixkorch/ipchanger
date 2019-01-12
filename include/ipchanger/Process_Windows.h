#ifndef WINDOWSPROCESS_HPP
#define WINDOWSPROCESS_HPP

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include <vector>

namespace ipchanger::system {

class ProcessBase {
private:
	HANDLE _handle;
	int _pid;
public:
    ProcessBase(int pid)
        : _pid(pid), _handle(nullptr)
    {}

    ProcessBase(const std::string& process_name)
        : _pid(FindPid(process_name + ".exe"))
    {}

    ~ProcessBase()
    {
        Detach();
    }

    int FindPid(const std::string& process_name) const
    {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        //std::wstring widestr = std::wstring(process_name.begin(), process_name.end());
        //const wchar_t* process = widestr.c_str();

        if (Process32First(snapshot, &entry) == TRUE)
        {
            while (Process32Next(snapshot, &entry) == TRUE)
            {
                if (lstrcmpi(entry.szExeFile, process_name.c_str()) == 0)
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

    int GetPid() const
    {
        return _pid;
    }

    int Attach() // Tries to attach to a process, usually requires administrator rights
    {
        DWORD access =
            PROCESS_VM_READ |
            PROCESS_QUERY_INFORMATION |
            PROCESS_VM_WRITE |
            PROCESS_VM_OPERATION;
        _handle = OpenProcess(access, FALSE, _pid);
		if(!_handle)
			return 0;
		return 1;
    }

    int Detach() const
    {
        CloseHandle(_handle);
        return 1;
    }

    int WriteToAddress(unsigned int addr, const char* buff, unsigned int length) const
    {
        if (!_handle) {
            std::cout << "Proccess must be attached." << std::endl;
            return 0;
        }

        SIZE_T written_bytes;
        WriteProcessMemory(_handle, (LPVOID)addr, buff, length, &written_bytes);
        return 1;
    }

    int ReadFromAddress(unsigned int addr, char* buff, unsigned int length) const
    {
        if (!_handle) {
            std::cout << "Proccess must be attached." << std::endl;
            return 0;
        }
        SIZE_T read_bytes;
        ReadProcessMemory(_handle, (LPCVOID)addr, buff, length, &read_bytes);
        return 1;
    }

    std::vector<char> ReadFromAddress(unsigned int addr, unsigned int length) const
    {
        if (!_handle) {
            std::cout << "Proccess must be attached." << std::endl;
            return std::vector<char>(length, 0);
        }

        std::vector<char> buff(length);
        SIZE_T read_bytes;
        ReadProcessMemory(_handle, (LPCVOID)addr, buff.data(), length, &read_bytes);
        return buff;
    }

};

} // end ipchanger::system

#endif // WINDOWSPROCESS_HPP
