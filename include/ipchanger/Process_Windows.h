#ifndef WINDOWSPROCESS_HPP
#define WINDOWSPROCESS_HPP

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
//#include <regex>
#include <optional>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace ipchanger::system {

class StreamMatcher { // Class to match for strings while reading memory continously
private:
    std::deque<char> _queue;
    std::string _pattern;
    bool _done;
    unsigned int _match_count;
    unsigned int _bytes_read;
    std::vector<unsigned int> _matches;

    std::condition_variable cv;
    std::mutex cv_m;
public:
    static constexpr std::size_t MAX_SIZE = 10000; // 10.000 bytes
public:
    StreamMatcher(const std::string& pattern)
        : _queue(0), _pattern(pattern), _done(false), _match_count(0), _bytes_read(0), _matches(0)
    {}

    int AddToQueue(const std::vector<char> data)
    {
        if (data.size() > (MAX_SIZE - _queue.size()))
            return 0;

        _queue.insert(std::end(_queue), std::begin(data), std::end(data));
        std::lock_guard<std::mutex> lk(cv_m);
        cv.notify_one();
        return 1;
    }

    std::thread Start()
    {
        std::cout << "Matcher started!" << std::endl;
        return std::thread(&StreamMatcher::Worker, this);
    }

    unsigned int NumberOfMatches()
    {
        return _match_count;
    }

    unsigned int BytesRead()
    {
        return _bytes_read;
    }

    std::vector<unsigned int> Matches()
    {
        return _matches;
    }

    void Worker()
    {
        std::unique_lock<std::mutex> lk(cv_m);
        unsigned int position = 0;
        unsigned int first_letter = 0;
        char current_char;

        while (!_done || !_queue.empty()) {
            if (_queue.empty()) { // While queue is empty, wait for signal from handler
                cv.wait(lk);
                continue;
            }

            current_char = _pattern[position];
            if(_queue.front() == 0) first_letter = _bytes_read + 1;

            if (current_char == _queue.front()) {
                if (position == _pattern.length() - 1) { // Actual length of string
                    _match_count++;
                    _matches.push_back(first_letter);
                    position = 0;
                }
                position++;
            }
            else {
                position = 0;
            }

            _queue.pop_front();
            _bytes_read++;
        }
        std::cout << "Matcher done" << std::endl;
    }

    void Stop()
    {
        std::lock_guard<std::mutex> lk(cv_m);
        _done = true;
        cv.notify_one();
        std::cout << "Stopping" << std::endl;
    }
};

class Process {
private:
	HANDLE _handle;
	int _pid;
public:
    Process(int pid)
        : _pid(pid), _handle(nullptr)
    {}

    Process(const std::string& process_name)
        : _pid(FindPid(process_name + ".exe"))
    {}

    ~Process()
    {
        Detach();
    }

    int FindPid(const std::string& process_name) const
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
                if (lstrcmpi(entry.szExeFile, process) == 0)
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

    std::vector<unsigned int> SearchMemoryString
        (const std::string& pattern, unsigned int start, unsigned int stop)
    {
        const unsigned int chunk = StreamMatcher::MAX_SIZE;
        const unsigned int count = stop - start + 1; // Number of addresses to read

        StreamMatcher sm(pattern);
        auto worker = sm.Start();

        using clock = std::chrono::system_clock;
        using ms = std::chrono::milliseconds;
        const auto before = clock::now(); // Measure time

        for (unsigned int i = start; i < stop;) {

            unsigned int size = chunk;
            if(chunk > count) size = count;

            std::vector<char> buff(size);
            ReadFromAddress(i, buff.data(), size);
            while(!sm.AddToQueue(buff));

            i += size;
        }

        sm.Stop();
        worker.join();

        const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);
        std::cout << "Time elapsed: " << duration.count() / 1000.0f << " seconds." << std::endl;

        std::vector<unsigned int> addresses;
        for(auto m : sm.Matches()) // Matcher returns offset from the start address therefore m + start
            addresses.push_back(m + start);
        return addresses;
    }

    void PrintMemory(unsigned int addr, unsigned int size) // Print memory as a string
    {
        std::vector<char> buff(size);
        ReadFromAddress(addr, buff.data(), size);
        std::cout << buff.data() << std::endl;
    }
};

} // end ipchanger namespace

#endif // WINDOWSPROCESS_HPP
