#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

class FlagTable
{
private:
    char name[0x20]{ 0 };
    std::array<uint32_t, 0x20> flags{ 0 }; // 1024 flags

public:
    FlagTable(std::string tableName)
    {
        std::copy_n(tableName.begin(), std::min(tableName.length(), sizeof(name) - 1), std::begin(name));
        name[sizeof(name) - 1] = '\0';
    }

    auto getName() { return name; }

    bool get(uint32_t flag)
    {
        if (flag >= (flags.size() << 5)) return false;
        return (flags[flag >> 5] >> (flag & 0x1F) & 1) != 0;
    }
    void clear(uint32_t flag)
    {
        if (flag >= (flags.size() << 5)) return;
        flags[flag >> 5] &= ~(1 << (flag & 0x1F));
    }
    void set(uint32_t flag)
    {
        if (flag >= (flags.size() << 5)) return;
        flags[flag >> 5] |= (1 << (flag & 0x1F));
    }
};
