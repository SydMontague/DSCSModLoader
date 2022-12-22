#include "ScriptExtension.h"

#include "dscs/GameInterface.h"
#include "modloader/plugin.h"

namespace dscs
{
    namespace digimon
    {
        int64_t GetScan(int64_t digimonId)
        {
            auto context = getGameContext();

            auto& digimonContext =
                context->storyMode->mode == StoryModeEnum::HACKERS_MEMORY ? context->digimonHM : context->digimonCS;
            auto& scanData = digimonContext->scanData;
            auto found     = scanData.find(digimonId);

            return found == scanData.end() ? 0 : found->second->scanrate;
        }

        void AddScan(int64_t digimonId, int64_t scan)
        {
            auto context = getGameContext();

            auto& digimonContext =
                context->storyMode->mode == StoryModeEnum::HACKERS_MEMORY ? context->digimonHM : context->digimonCS;
            auto& scanData = digimonContext->scanData;
            auto found     = scanData.find(digimonId);

            if (found == scanData.end()) return;

            int64_t newScan = found->second->scanrate + scan;

            if (newScan > 200)
                newScan = 200;
            else if (newScan < 0)
                newScan = 0;

            found->second->scanrate = (uint16_t)newScan;
        }

        void SetScan(int64_t digimonId, int64_t scan)
        {
            auto context = getGameContext();

            auto& digimonContext =
                context->storyMode->mode == StoryModeEnum::HACKERS_MEMORY ? context->digimonHM : context->digimonCS;
            auto& scanData = digimonContext->scanData;
            auto found     = scanData.find(digimonId);

            if (found == scanData.end()) return;

            uint64_t newScan = scan;

            if (newScan > 200)
                newScan = 200;
            else if (newScan < 0)
                newScan = 0;

            found->second->scanrate = (uint16_t)newScan;
        }
    } // namespace digimon

    namespace modloader
    {
        void SetFlag(const SQChar* name, int32_t flag)
        {
            std::string string{ name };
            DSCSModLoader::getInstance().getFlagTable(string).set(flag);
        }
        bool GetFlag(const SQChar* name, int32_t flag)
        {
            std::string string{ name };
            return DSCSModLoader::getInstance().getFlagTable(string).get(flag);
        }
        void ClearFlag(const SQChar* name, int32_t flag)
        {
            std::string string{ name };
            DSCSModLoader::getInstance().getFlagTable(string).clear(flag);
        }

        void StorageSetInt(const SQChar* name, int32_t flag, int32_t value)
        {
            std::string string{ name };
            DSCSModLoader::getInstance().getWorkTable(string).setInt(flag, value);
        }
        void StorageSetFloat(const SQChar* name, int32_t flag, float value)
        {
            std::string string{ name };
            DSCSModLoader::getInstance().getWorkTable(string).setFloat(flag, value);
        }
        int32_t StorageGetInt(const SQChar* name, int32_t flag)
        {
            std::string string{ name };
            return DSCSModLoader::getInstance().getWorkTable(string).getInt(flag);
        }
        float StorageGetFloat(const SQChar* name, int32_t flag)
        {
            std::string string{ name };
            return DSCSModLoader::getInstance().getWorkTable(string).getFloat(flag);
        }

        void DebugLog(HSQUIRRELVM vm, const SQChar* msg) { sq_getprintfunc(vm)(vm, msg); }

    } // namespace modloader
} // namespace dscs
