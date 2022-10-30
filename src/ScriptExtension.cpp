#include "ScriptExtension.h"

#include "dscs/GameInterface.h"

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
} // namespace dscs