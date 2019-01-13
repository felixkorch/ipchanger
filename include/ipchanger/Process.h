#ifndef PROCESS_H
#define PROCESS_H

//#include <regex>
#include <optional>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>

#ifdef _WIN32

#include "ipchanger/Process_Windows.h"

#else

#include "ipchanger/Process_Unix.h"


#endif // WIN32

namespace ipchanger::system {

class StreamMatcher { // Class to match for strings while reading memory continously  

private:
    std::deque<char> _queue;
    std::string _pattern;
    bool _done;
    std::size_t _bytes_read;
    std::vector<std::uintptr_t> _matches;
    std::function<void(std::uintptr_t)> _callback;

    std::condition_variable cv;
    std::mutex cv_m;
public:
    static constexpr std::size_t MAX_SIZE = 10000; // 10.000 bytes

public:
    StreamMatcher(const std::string& pattern, std::function<void(std::uintptr_t)> callback = nullptr)
        : _queue(0), _pattern(pattern), _done(false), _bytes_read(0), _matches(0), _callback(callback)
    {}

    int AddToQueue(const std::vector<char> data)
    {
        std::unique_lock<std::mutex> lk(cv_m);

        if (data.size() > (MAX_SIZE - _queue.size()))
            return 0;

        _queue.insert(std::end(_queue), std::begin(data), std::end(data));
        lk.unlock();
        cv.notify_one();
        return 1;
    }

    std::thread Start()
    {
        std::cout << "Matcher started!" << std::endl;
        return std::thread(&StreamMatcher::Worker, this);
    }

    std::size_t NumberOfMatches()
    {
        return _matches.size();
    }

    std::size_t BytesRead()
    {
        return _bytes_read;
    }

    std::vector<std::uintptr_t> Matches()
    {
        return _matches;
    }

    void Worker()
    {
        unsigned int position = 0;
        std::uintptr_t first_letter = 0;
        char current_char;

        while (!_done || !_queue.empty()) {

            std::unique_lock<std::mutex> lk(cv_m);

            if (_queue.empty()) { // While queue is empty, wait for signal from handler
                cv.wait(lk);
                continue;
            }

            current_char = _pattern[position];
            if(_queue.front() == 0)
                first_letter = _bytes_read + 1;

            if (current_char == _queue.front()) {
                if (position == _pattern.length() - 1) { // Actual length of string
                    _matches.push_back(first_letter);
                    if(_callback) _callback(first_letter);
                    position = 0;
                }else{
                    position++;
                }
            }
            else {
                position = 0;
            }

            _queue.pop_front();
            _bytes_read++;

            lk.unlock();
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


class Process : public ProcessBase {
public:

    Process(const std::string& process_name)
        : ProcessBase(process_name) {}

    Process(int pid)
        : ProcessBase(pid) {}

    std::vector<std::uintptr_t> SearchMemoryString
        (const std::string& pattern, std::uintptr_t start,
         std::uintptr_t stop,
         std::function<void(std::uintptr_t)> cb = nullptr)
    {
        std::size_t chunk = StreamMatcher::MAX_SIZE / 2;
        std::size_t count = stop - start + 1; // Number of addresses to read

        StreamMatcher sm(pattern, cb);
        auto worker = sm.Start();

        using clock = std::chrono::system_clock;
        using ms = std::chrono::milliseconds;
        const auto before = clock::now(); // Measure time

        for (std::size_t i = start; i < stop;) {

            std::size_t size = chunk;
            if(chunk > count) size = count;

            std::vector<char> buff(size);
            ReadFromAddress(i, buff.data(), size);
            while(!sm.AddToQueue(buff));

            i += size;
        }

        sm.Stop(); // End matcher process
        worker.join();

        const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);
        std::cout << "Time elapsed: " << duration.count() / 1000.0f << " seconds." << std::endl;

        return sm.Matches();
    }

    template <class T>
    void PrintMemory(std::uintptr_t addr, std::size_t size)
    {
        std::vector<T> buff(size / sizeof(T));
        ReadFromAddress(addr, reinterpret_cast<char*>(buff.data()), size);

        std::stringstream out;

        for(T t : buff)
            out << t;
        out << std::endl;

        std::cout << out.str();
    }
};

} // end ipchanger::system

#endif // PROCESS_H
