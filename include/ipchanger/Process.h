#ifndef PROCESS_H
#define PROCESS_H

#ifdef _WIN32

#include "ipchanger/Process_Windows.h"

#else

#include "ipchanger/Process_Unix.h"


#endif // WIN32

#include "ipchanger/System.h"

namespace ipchanger::system {

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
         std::function<void(std::uintptr_t match, std::uintptr_t word)> cb = nullptr)
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

            std::vector<T> buff = ReadFromAddress<T>(i, size);
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

} // namespace

#endif // PROCESS_H
