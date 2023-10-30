#include "CoSaveExtension.hpp"

#include "dscs/GameInterface.h"

#include <iterator>

// force Windows 7 WINAPI, so Boost::log works. Seems like a Boost bug
#define BOOST_USE_WINAPI_VERSION 0x0601
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>

void readSeenData(std::vector<uint8_t> data)
{
    // don't do anything with empty data
    if (data.empty()) return;

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
        if (seenData->contains(entry.entryId)) *seenData->at(entry.entryId) = entry;
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
    // don't do anything with empty data
    if (data.empty()) return;

    struct LocalScanData
    {
        uint32_t size;
        uint32_t version;
        dscs::ScanData data[0];
    };

    auto context             = dscs::getGameContext();
    auto digimon             = isHM ? context->digimonHM : context->digimonCS;
    auto scanData            = digimon->scanData;
    LocalScanData* localData = reinterpret_cast<LocalScanData*>(data.data());

    /*
     * In version 0 the mod loader would load/store the scan data from HM for both games.
     * When upgrading to a mod loader version that fixes this all scan data would be lost
     */
    if (localData->version == 0 && !isHM)
    {
        BOOST_LOG_TRIVIAL(info) << "Found old v0 CS scan data. Skipping, since it was invalid.";
        return;
    }

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
    auto digimon     = isHM ? context->digimonHM : context->digimonCS;
    auto scanData    = digimon->scanData;
    uint32_t size    = scanData.size();
    uint32_t version = 1;

    std::copy_n(reinterpret_cast<uint8_t*>(&size), sizeof(size), std::back_inserter(data));
    std::copy_n(reinterpret_cast<uint8_t*>(&version), sizeof(version), std::back_inserter(data));

    for (auto& entry : scanData)
        std::copy_n(reinterpret_cast<uint8_t*>(entry.second), sizeof(*entry.second), std::back_inserter(data));

    return data;
}

std::vector<uint8_t> writeScanDataCS() { return writeScanData(false); }
std::vector<uint8_t> writeScanDataHM() { return writeScanData(true); }

void readScanDataCS(std::vector<uint8_t> data) { readScanData(data, false); }
void readScanDataHM(std::vector<uint8_t> data) { readScanData(data, true); }
