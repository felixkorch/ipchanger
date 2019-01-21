#ifndef SYSTEM_H
#define SYSTEM_H

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <vector>
#include <functional>
#include "boost/filesystem.hpp"
#include <iostream>

namespace ipchanger::system {

enum class OS { Linux, Mac, Windows };

#ifdef __linux__
constexpr OS current_os = OS::Linux;
#elif __APPLE__
constexpr OS current_os = OS::Mac;
#elif __WIN32
constexpr OS current_os = OS::Windows;
#endif

struct AsciiNum {
    char c;

    AsciiNum()
        : c('0') {}

    AsciiNum(char c)
        : c(c) {}

    bool operator== (AsciiNum&& other)
    {
        if(c == 0 && other.c == 0)
            return true;

        if(other.c >= 48 && other.c <= 57)
            return true;
        return false;
    }

    bool operator== (AsciiNum& other)
    {
        if(c == 0 && other.c == 0)
            return true;

        if(other.c >= 48 && other.c <= 57)
            return true;
        return false;
    }
};

template <class T>
class StreamMatcher { // Class to match patterns while reading memory continously

private:
    std::condition_variable cv;
    std::mutex cv_m;

    std::deque<T> _queue;
    bool _done;
    std::size_t _bytes_read;
    std::vector<std::uintptr_t> _offsets_word; // Offset to first non-zero byte before the sequence
    std::vector<std::uintptr_t> _offsets_match; // Offset to match
    std::function<void(std::uintptr_t, std::uintptr_t)> _callback;
    std::thread _worker;

public:
    StreamMatcher(const std::vector<T> pattern, std::function<void(std::uintptr_t, std::uintptr_t)> callback = nullptr)
        : _done(false), _bytes_read(0), _callback(callback), _worker(&StreamMatcher::Worker, this, pattern) {}

    template <class K>
    void AddToQueue(const std::vector<K> data)
    {
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
        std::size_t next_word = 0;
        unsigned int position = 0;

        while(!_done || !_queue.empty()) {
            std::unique_lock<std::mutex> lk(cv_m);

            if(_queue.empty() && !_done) {
                cv.wait(lk);
                continue;
            }

            T current = pattern[position];
            T front = _queue.front();
            _queue.pop_front();
            _bytes_read++;

            if(front == 0)
                next_word = _bytes_read;

            if(current == front)
                position++;
            else
                position = 0;

            if(position == pattern.size()) {
                _offsets_word.push_back(next_word);
                _offsets_match.push_back(_bytes_read - 1);
                if(_callback) _callback(_bytes_read - 1, next_word);
                position = 0;
            }
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
    }
};

} // namespace

#endif // SYSTEM_H