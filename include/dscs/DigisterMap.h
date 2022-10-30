#pragma once
#include <stdint.h>

#include <map>
#include <string>

namespace dscs
{

    struct DigisterData
    {
        uint8_t unknown[0x38];
        std::map<std::string, std::string> map;
    };

    struct DigisterMap
    {
        void* functions;
        std::map<std::string, DigisterData*> map;
    };

} // namespace dscs