#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <optional>
#include <regex>

namespace ipchanger {

struct Client
{
        const unsigned int version; // Tibia version
        const unsigned int write_address; // Start address
        const unsigned int step;
        const unsigned int port_offset;
        const unsigned int rsa_address;
        const unsigned int count;
};

constexpr auto RSA_KEY =
        "10912013296739942927886096050899554152823750290279"
        "81291234687579372662914925764463307396960011106039"
        "07230888610072655818825358503429057592827629436413"
        "10856602909362821263595383668656267584972062078627"
        "94310902180176810615217550567108238764764442605581"
        "47179707119674283982419152118103759076030616683978566631413";

constexpr Client clients[] = {
    {790, 0x86ba620, 0x70, 0x64, 0x84ab8c0, 3},  {791, 0x867c9a0, 0x70, 0x64, 0x84a6fe0, 3},
    {792, 0x867cde0, 0x70, 0x64, 0x84a6ec0, 3},  {800, 0x8694b20, 0x70, 0x64, 0x84bccc0, 10},
    {810, 0x866ef00, 0x70, 0x64, 0x8483440, 10}, {811, 0x866ed40, 0x70, 0x64, 0x8483280, 10},
    {820, 0x869a900, 0x70, 0x64, 0x84a91c0, 10}, {821, 0x86a4540, 0x70, 0x64, 0x84b2c00, 10},
    {822, 0x86a7d60, 0x70, 0x64, 0x84b5240, 10}, {830, 0x86b2d80, 0x70, 0x64, 0x84bf3a0, 10},
    {831, 0x86b4d80, 0x70, 0x64, 0x84c0e40, 10}, {840, 0x86b8c00, 0x70, 0x64, 0x84c1040, 10},
    {841, 0x8579c40, 0x70, 0x64, 0x8380580, 10}, {842, 0x8569d00, 0x70, 0x64, 0x83753a0, 10},
    {850, 0x856bd00, 0x70, 0x64, 0x8376a40, 10}, {852, 0x856efe0, 0x70, 0x64, 0x8378f40, 10},
    {853, 0x8571180, 0x70, 0x64, 0x837aaa0, 10}, {854, 0x8572ce0, 0x70, 0x64, 0x837b720, 10},
    {855, 0x857fd40, 0x70, 0x64, 0x8386a00, 10}, {857, 0x85818c0, 0x70, 0x64, 0x8388ee0, 10},
    {860, 0x85828e0, 0x70, 0x64, 0x8389a40, 10}, {861, 0x856e8a0, 0x70, 0x64, 0x83781c0, 10},
    {862, 0x859b860, 0x70, 0x64, 0x837a8e0, 10}, {870, 0x85acde0, 0x70, 0x64, 0x8388480, 10},
    {871, 0x85acde0, 0x70, 0x64, 0x8388420, 10}, {872, 0x85adda0, 0x70, 0x64, 0x83898c0, 10},
    {971, 0x85de0e0, 0x70, 0x64, 0x8388420, 10}
};

auto constexpr REGEX_DOMAIN_NAME = R"((([a-zA-Z0-9]{1,63}|[a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9])\.){0,3}[a-zA-Z]{2,63}\b(?!\.))";

inline std::optional<Client> FindClient(unsigned int version)
{
    for (unsigned int i = 0; i < std::size(clients); i++) {
        if (clients[i].version == version)
            return std::make_optional(clients[i]);
    }
    return std::nullopt;
}

} // end namespace ipchanger

#endif // CLIENT_HPP
