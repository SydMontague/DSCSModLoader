#include "modloader/utils.h"
#include <sstream>
#include <Windows.h>

char* getBaseOffset() {
	static char* offset = (char*)GetModuleHandle(NULL);
	return offset;
}

void patchBytes(const std::vector<uint8_t>& data, const uint64_t offset) {
	DWORD flag = 0;
	size_t size = data.size();

	VirtualProtect(getBaseOffset() + offset, size, PAGE_EXECUTE_READWRITE, &flag);
	std::copy_n(data.data(), size, getBaseOffset() + offset);
	VirtualProtect(getBaseOffset() + offset, size, flag, &flag);
}

void redirectJump(const void* target, const uint64_t offset) {
	const std::vector<uint8_t> injectionData = {
		(uint8_t)0x48, (uint8_t)0xB8, ptr2bytes(target),	// MOV RAX,target
		(uint8_t)0xFF, (uint8_t)0xE0,						// JMP RAX
	};

	patchBytes(injectionData, offset);
}

void redirectCall(const void* target, const uint64_t offset) {
	const std::vector<uint8_t> injectionData = {
		(uint8_t)0x48, (uint8_t)0xB8, ptr2bytes(target),	// MOV RAX,target
		(uint8_t)0xFF, (uint8_t)0xD0,						// CALL RAX
	};

	patchBytes(injectionData, offset);
}

const HexToStringResult parseByteArray(const std::string& str, std::vector<uint8_t>& vector) {
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
