#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <stdint.h>

void patchBytes(const std::vector<uint8_t>& data, const uint64_t offset) {
	DWORD flag = 0;
	size_t size = data.size();

	VirtualProtect(BASE_ADDRESS + offset, size, PAGE_EXECUTE_READWRITE, &flag);
	std::copy_n(data.data(), size, BASE_ADDRESS + offset);
	VirtualProtect(BASE_ADDRESS + offset, size, flag, &flag);
}

void redirectJump(const void* target, const uint64_t offset) {
	std::vector<uint8_t> injectionData = {
		(uint8_t)0x48, (uint8_t)0xB8, ptr2bytes(target),	// MOV RAX,target
		(uint8_t)0xFF, (uint8_t)0xE0,						// JMP RAX
	};

	patchBytes(injectionData, offset);
}

enum class HexToStringResult {
	SUCCESS,
	WRONG_TOKEN_SIZE,
	INVALID_TOKEN
};

HexToStringResult parseByteArray(const std::string& str, std::vector<uint8_t>& vector) {
	std::stringstream sstream(str);
	std::vector<uint8_t> local;

	while (!sstream.eof()) {
		std::string val;
		sstream >> val;

		if (val.size() != 2)
			return HexToStringResult::WRONG_TOKEN_SIZE;

		std::size_t pos;
		local.push_back(std::stoi(val, &pos, 16));

		if (pos != val.size())
			return HexToStringResult::INVALID_TOKEN;
	}

	vector.insert(vector.end(), local.begin(), local.end());
	return HexToStringResult::SUCCESS;
}
