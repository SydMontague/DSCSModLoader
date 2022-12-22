#pragma once
#include "modloader/squirrel.h"

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
        void SetFlag(const SQChar* name, int32_t flag);
        bool GetFlag(const SQChar* name, int32_t flag);
        void ClearFlag(const SQChar* name, int32_t flag);

        void StorageSetInt(const SQChar* name, int32_t flag, int32_t value);
        void StorageSetFloat(const SQChar* name, int32_t flag, float value);
        int32_t StorageGetInt(const SQChar* name, int32_t flag);
        float StorageGetFloat(const SQChar* name, int32_t flag);

        void DebugLog(HSQUIRRELVM vm, const SQChar* msg);
    } // namespace modloader
} // namespace dscs
