#ifndef CHANGER_H
#define CHANGER_H

#include "ipchanger/System.h"
#include <boost/filesystem.hpp>
#include <string>
#include <algorithm>

namespace ipchanger {

namespace fs = boost::filesystem;

class Changer {
private:
    static constexpr std::string_view RSA_KEY{
            "10912013296739942927886096050899554152823750290279"
            "81291234687579372662914925764463307396960011106039"
            "07230888610072655818825358503429057592827629436413"
            "10856602909362821263595383668656267584972062078627"
            "94310902180176810615217550567108238764764442605581"
            "47179707119674283982419152118103759076030616683978566631413"};

    static constexpr std::string_view p_old[]
        { "server1.tibia.com", "server2.tibia.com",
          "tibia1.cipsoft.com", "tibia1.cipsoft.com" };

    static constexpr std::string_view p_new[]
        { "login0", "tibia0" };

    static constexpr std::string_view rsa1{ "1321277432058722840622950990822933849527763264961655079678763" }; // First try to find this RSA
    static constexpr std::string_view rsa2{ "124710459426827943004376449897985582167801707960697037164044904" }; // Fallback RSA (pre 8.61)
private:
    std::string buffer;
private:
    bool SearchAndReplace(std::string& in, const std::string_view& pattern, const std::string_view& replace)
    {
        auto search = std::search(in.begin(), in.end(), std::boyer_moore_searcher(pattern.begin(), pattern.end()));
        if(search == in.end())
            return false; // No match
        while(search != in.end()) {
            std::copy(replace.data(), replace.data() + replace.size() + 1, search);
            search = std::search(search, in.end(), std::boyer_moore_searcher(pattern.begin(), pattern.end()));
        }
        return true; // One or more matches
    }
public:
    Changer(const std::string& ip, unsigned int port, const fs::path& in)
        : buffer{ system::ReadFile<std::string>(in) }
    {
        bool new_client = false;

        for(auto p : p_new)
            new_client = SearchAndReplace(buffer, p, ip);

        if(!new_client)
            for(auto p : p_old) SearchAndReplace(buffer, p, ip);

        if(!SearchAndReplace(buffer, rsa1, RSA_KEY))
            SearchAndReplace(buffer, rsa2, RSA_KEY);
    }

    const char* Data() const
    {
        return buffer.data();
    }

    std::size_t Size() const
    {
        return buffer.size();
    }
};

} // namespace

#endif // CHANGER_H
