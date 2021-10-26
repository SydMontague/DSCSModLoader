#pragma once
#include <stdint.h>
#include <vector>
#include <string>

//#define BASE_ADDRESS (char*)GetModuleHandle(NULL)
#define ptr2bytes(ptr) ((uint8_t*) &ptr)[0],((uint8_t*) &ptr)[1],((uint8_t*) &ptr)[2],((uint8_t*) &ptr)[3],((uint8_t*) &ptr)[4],((uint8_t*) &ptr)[5],((uint8_t*) &ptr)[6],((uint8_t*) &ptr)[7]

enum class HexToStringResult {
	SUCCESS,
	WRONG_TOKEN_SIZE,
	INVALID_TOKEN
};

extern __declspec(dllexport) char* getBaseOffset();

extern __declspec(dllexport) void patchBytes(const std::vector<uint8_t>& data, const uint64_t offset);

extern __declspec(dllexport) void redirectJump(const void* target, const uint64_t offset);

extern __declspec(dllexport) void redirectCall(const void* target, const uint64_t offset);

extern __declspec(dllexport) const HexToStringResult parseByteArray(const std::string& str, std::vector<uint8_t>& vector);
