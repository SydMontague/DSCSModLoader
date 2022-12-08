#pragma once
#include <stdint.h>

#include <string>

namespace dscs
{
    namespace digimon
    {
        int64_t GetScan(int64_t digimonId);
        void AddScan(int64_t digimonId, int64_t scan);
        void SetScan(int64_t digimonId, int64_t scan);
    } // namespace digimon

    namespace modloader
    {
        void SetFlag(std::string name, int32_t flag);
        bool GetFlag(std::string name, int32_t flag);
        void ClearFlag(std::string name, int32_t flag);

        void StorageSetInt(std::string name, int32_t flag, int32_t value);
        void StorageSetFloat(std::string name, int32_t flag, float value);
        int32_t StorageGetInt(std::string name, int32_t flag);
        float StorageGetFloat(std::string name, int32_t flag);
    } // namespace modloader
} // namespace dscs
