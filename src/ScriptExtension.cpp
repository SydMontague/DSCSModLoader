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
        void SetFlag(std::string name, int32_t flag) { DSCSModLoader::getInstance().getFlagTable(name).set(flag); }
        bool GetFlag(std::string name, int32_t flag)
        {
            return DSCSModLoader::getInstance().getFlagTable(name).get(flag);
        }
        void ClearFlag(std::string name, int32_t flag) { DSCSModLoader::getInstance().getFlagTable(name).clear(flag); }

        void StorageSetInt(std::string name, int32_t flag, int32_t value)
        {
            DSCSModLoader::getInstance().getWorkTable(name).setInt(flag, value);
        }
        void StorageSetFloat(std::string name, int32_t flag, float value)
        {
            DSCSModLoader::getInstance().getWorkTable(name).setFloat(flag, value);
        }
        int32_t StorageGetInt(std::string name, int32_t flag)
        {
            return DSCSModLoader::getInstance().getWorkTable(name).getInt(flag);
        }
        float StorageGetFloat(std::string name, int32_t flag)
        {
            return DSCSModLoader::getInstance().getWorkTable(name).getFloat(flag);
        }
    } // namespace modloader
} // namespace dscs
