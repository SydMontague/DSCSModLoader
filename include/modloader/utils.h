#pragma once
#include <stdint.h>

#include <string>
#include <vector>

#define ptr2bytes(ptr)                                                                                                 \
    ((uint8_t*)&ptr)[0], ((uint8_t*)&ptr)[1], ((uint8_t*)&ptr)[2], ((uint8_t*)&ptr)[3], ((uint8_t*)&ptr)[4],           \
        ((uint8_t*)&ptr)[5], ((uint8_t*)&ptr)[6], ((uint8_t*)&ptr)[7]

enum class HexToStringResult
{
    SUCCESS,
    WRONG_TOKEN_SIZE,
    INVALID_TOKEN
};

extern __declspec(dllexport) char* getBaseOffset();

extern __declspec(dllexport) void patchBytes(const std::vector<uint8_t>& data, const uint64_t offset);

extern __declspec(dllexport) void redirectJump(const void* target, const uint64_t offset);

extern __declspec(dllexport) void redirectCall(const void* target, const uint64_t offset);

extern __declspec(dllexport) const HexToStringResult
    parseByteArray(const std::string& str, std::vector<uint8_t>& vector);

template<typename Input, typename Output, typename Extractor>
void copy_max(Input& container, Output output, std::size_t max, Extractor func)
{
    uint32_t count = 0;
    for (auto it = container.begin(); count < max && it != container.end(); count++, output++, it++)
        *output = func(it);
}

template<typename Input, typename Output, typename Extractor>
void copy_max(Input* container, Output output, std::size_t max, Extractor func)
{
    copy_max(*container, output, max, func);
}