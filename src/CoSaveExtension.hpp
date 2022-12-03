#pragma once

#include <stdint.h>

#include <vector>

std::vector<uint8_t> writeScanDataCS();
std::vector<uint8_t> writeScanDataHM();

void readScanDataCS(std::vector<uint8_t> data);
void readScanDataHM(std::vector<uint8_t> data);

std::vector<uint8_t> writeSeenData();
void readSeenData(std::vector<uint8_t> data);
