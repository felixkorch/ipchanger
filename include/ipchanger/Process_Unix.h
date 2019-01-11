#ifndef UNIXPROCESS_HPP
#define UNIXPROCESS_HPP

#include "ipchanger/Client.h"
#include "boost/process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string>
#include <iostream>
//#include <regex>
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
    int _pid;
public:
    Process(int pid)
        : _pid(pid)
    {}

    Process(const std::string& process_name)
        : _pid(FindPid(process_name))
    {}

    ~Process()
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

    int WriteToAddress(unsigned int addr, const char* buff, unsigned int length) const
    {
        for (unsigned int i = 0; i < length; i++) {
            ptrace(PTRACE_POKEDATA, _pid, addr + i, buff[i]);
        }
        return 1;
    }

    int ReadFromAddress(unsigned int addr, char* buff, unsigned int length) const
    {
        for (unsigned int i = 0; i < length; i++) {
            buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
        }
        return 1;
    }

    std::vector<char> ReadFromAddress(unsigned int addr, unsigned int length) const
    {
        std::vector<char> buff(length);
        for (unsigned int i = 0; i < length; i++) {
            buff[i] = ptrace(PTRACE_PEEKDATA, _pid, addr + i, 0);
        }
        return buff;
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

#endif // UNIXPROCESS_HPP
