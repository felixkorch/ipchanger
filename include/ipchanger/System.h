#ifndef SYSTEM_H
#define SYSTEM_H

#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <sstream>
#include <vector>
#include <iostream>

namespace ipchanger::system {

enum class OS { Linux, Mac, Windows };

#ifdef __linux__
constexpr OS current_os = OS::Linux;
#elif __APPLE__
constexpr OS current_os = OS::Mac;
#elif _WIN32
constexpr OS current_os = OS::Windows;

#include <windows.h>
#include <winnt.h>
#include <fileapi.h>

#endif

constexpr unsigned long WINDOWS_HIDDEN_FILE = 0x2;
constexpr unsigned long WINDOWS_NORMAL_FILE = 0x80;

namespace fs = boost::filesystem;
namespace ps = boost::process;

template <class T>
inline T ReadFile(const fs::path& path)
{
    fs::ifstream in{ path, std::ios::binary };
    return T{ std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>() };
}

inline void WriteBinary(const fs::path& path, const char* buff, std::size_t length, unsigned long options = WINDOWS_NORMAL_FILE)
{
#ifdef _WIN32
    HANDLE handl = CreateFile(path.string().c_str(), GENERIC_WRITE, 0, nullptr,
    CREATE_ALWAYS, options, nullptr);
    WriteFile(handl, buff, length, nullptr, nullptr);
    CloseHandle(handl);
#else
    fs::ofstream output{ path, std::ios::binary };
    output.write(buff, length);
    fs::permissions(path, fs::owner_all); // Allow all permissions to be able to execute it
#endif
}

inline void ExecuteBinary(const fs::path& path)
{
    if(!fs::exists(path))
        return;

    fs::current_path(path.parent_path());
    ps::system(path);
}

} // namespace

#endif // SYSTEM_H
