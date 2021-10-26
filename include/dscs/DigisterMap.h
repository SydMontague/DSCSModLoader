#pragma once
#include <stdint.h>
#include <dscs/lib/map.h>
#include <dscs/lib/string.h>

namespace dscs {

	struct DigisterData {
		uint8_t unknown[0x38];
		map<string, string> map;
	};

	struct DigisterMap {
		void* functions;
		map<string, DigisterData*> map;
	};

}