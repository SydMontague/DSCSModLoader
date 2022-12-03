#include "CoSaveExtension.hpp"

#include "dscs/GameInterface.h"

#include <iterator>

void readSeenData(std::vector<uint8_t> data)
{
    struct LocalSeenData
    {
        std::size_t size;
        dscs::SeenData data[0];
    };

    auto seenData            = dscs::getSeenData();
    LocalSeenData* localData = reinterpret_cast<LocalSeenData*>(data.data());

    for (std::size_t i = 0; i < localData->size; i++)
    {
        auto& entry = localData->data[i];
        if (entry.entryId != 0) *seenData->at(entry.entryId) = entry;
    }
}

std::vector<uint8_t> writeSeenData()
{
    std::vector<uint8_t> data;
    auto seenData    = dscs::getSeenData();
    std::size_t size = seenData->size();

    std::copy_n(reinterpret_cast<uint8_t*>(&size), sizeof(size), std::back_inserter(data));

    for (auto& entry : *seenData)
        std::copy_n(reinterpret_cast<uint8_t*>(entry.second), sizeof(*entry.second), std::back_inserter(data));

    return data;
}

void readScanData(std::vector<uint8_t> data, bool isHM)
{
    struct LocalScanData
    {
        std::size_t size;
        dscs::ScanData data[0];
    };

    auto context             = dscs::getGameContext();
    auto digimon             = isHM ? context->digimonHM : context->digimonHM;
    auto scanData            = digimon->scanData;
    LocalScanData* localData = reinterpret_cast<LocalScanData*>(data.data());

    for (std::size_t i = 0; i < localData->size; i++)
    {
        auto& scanEntry = localData->data[i];
        if (scanEntry.digimonId != 0) dscs::setScanData(digimon, scanEntry.digimonId, scanEntry.scanrate);
    }
}

std::vector<uint8_t> writeScanData(bool isHM)
{
    std::vector<uint8_t> data;
    auto context     = dscs::getGameContext();
    auto digimon     = isHM ? context->digimonHM : context->digimonHM;
    auto scanData    = digimon->scanData;
    std::size_t size = scanData.size();

    std::copy_n(reinterpret_cast<uint8_t*>(&size), sizeof(size), std::back_inserter(data));

    for (auto& entry : scanData)
        std::copy_n(reinterpret_cast<uint8_t*>(entry.second), sizeof(*entry.second), std::back_inserter(data));

    return data;
}

std::vector<uint8_t> writeScanDataCS() { return writeScanData(false); }
std::vector<uint8_t> writeScanDataHM() { return writeScanData(true); }

void readScanDataCS(std::vector<uint8_t> data) { readScanData(data, false); }
void readScanDataHM(std::vector<uint8_t> data) { readScanData(data, true); }
