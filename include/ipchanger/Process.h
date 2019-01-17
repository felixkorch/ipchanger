#ifndef PROCESS_H
#define PROCESS_H

//#include <regex>
#include <optional>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <iterator>

#ifdef _WIN32

#include "ipchanger/Process_Windows.h"

#else

#include "ipchanger/Process_Unix.h"


#endif // WIN32

namespace ipchanger::system {

template <class T>
class StreamMatcher { // Class to match for strings while reading memory continously  

private:
    std::deque<T> _queue;
    bool _done;
    std::size_t _bytes_read;
    std::vector<std::uintptr_t> _offsets_word; // Offset to first non-zero byte before the sequence
    std::vector<std::uintptr_t> _offsets_match; // Offset to match
    std::function<void(std::uintptr_t)> _callback;
    std::vector<T> _pattern;
    std::thread _worker;

    std::condition_variable cv;
    std::mutex cv_m;

public:
    StreamMatcher(const std::vector<T> pattern, std::function<void(std::uintptr_t)> callback = nullptr)
        : _done(false), _bytes_read(0), _callback(callback), _pattern(pattern), _worker(&StreamMatcher::Worker, this, _pattern)
    {
        std::cout << "Matcher started!" << std::endl;
    }

    void AddToQueue(const std::vector<T> data)
    {
        //while(_queue.size() > 10000)
        //    ;
        {
            std::lock_guard<std::mutex> lk(cv_m);
            _queue.insert(_queue.end(), data.begin(), data.end());
        }
        cv.notify_one();
    }

    std::size_t NumberOfMatches()
    {
        return _offsets_match.size();
    }

    std::size_t BytesRead()
    {
        return _bytes_read;
    }

    std::vector<std::uintptr_t> OffsetToMatch()
    {
        return _offsets_match;
    }

    std::vector<std::uintptr_t> OffsetToWord()
    {
        return _offsets_word;
    }

    void Worker(const std::vector<T> pattern)
    {
        unsigned int next_word = 0;
        unsigned int position = 0;

        while(!_done || !_queue.empty()) {
            std::unique_lock<std::mutex> lk(cv_m);

            if(_queue.empty())
                cv.wait(lk);

            T current = pattern[position];
            T front = _queue.front();
            _queue.pop_front();
            _bytes_read++;

            if(front == 0)
                next_word = _bytes_read;

            if(front == current)
                position++;
            else
                position = 0;

            if(position == pattern.size()) {
                _offsets_word.push_back(next_word);
                _offsets_match.push_back(_bytes_read - 1);
                if(_callback) _callback(next_word);
                position = 0;
            }

            lk.unlock();
        }
    }

    void Stop()
    {
        {
            std::lock_guard<std::mutex> lk(cv_m);
            _done = true;
        }
        cv.notify_one();
        _worker.join();
        std::cout << "Stopping" << std::endl;
    }
};


class Process : public ProcessBase {
public:

    Process(const std::string& process_name)
        : ProcessBase(process_name) {}

    Process(int pid)
        : ProcessBase(pid) {}

    template <class T>
    StreamMatcher<T>* SearchMemory
        (const std::vector<T> pattern, std::uintptr_t start,
         std::uintptr_t stop,
         std::function<void(std::uintptr_t adr)> cb = nullptr)
    {
        std::size_t chunk = 10000;
        std::size_t count = stop - start + 1; // Number of addresses to read

        auto sm = new StreamMatcher<T>(pattern, cb);

        using clock = std::chrono::system_clock;
        using ms = std::chrono::milliseconds;
        const auto before = clock::now(); // Measure time

        for (std::size_t i = start; i < stop;) {

            std::size_t size = chunk;
            if(chunk > count) size = count;
            auto buff = ReadFromAddress<T>(i, size);
            sm->AddToQueue(buff);

            i += size;
        }

        sm->Stop(); // End matcher process

        const auto duration = std::chrono::duration_cast<ms>(clock::now() - before);
        std::cout << "Time elapsed: " << duration.count() / 1000.0f << " seconds." << std::endl;

        return sm;
    }

    template <class T>
    void PrintMemory(std::uintptr_t addr, std::size_t size = 1)
    {
        auto buff = ReadFromAddress<T>(addr, size);
        std::stringstream out;

        for(T t : buff)
            out << t;
        out << std::endl;

        std::cout << out.str();
    }
};

} // end ipchanger::system

#endif // PROCESS_H
