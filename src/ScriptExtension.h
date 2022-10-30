#pragma once
#include <stdint.h>

namespace dscs {
	namespace digimon {
		int64_t GetScan(int64_t digimonId);
		void AddScan(int64_t digimonId, int64_t scan);
		void SetScan(int64_t digimonId, int64_t scan);
	}
}