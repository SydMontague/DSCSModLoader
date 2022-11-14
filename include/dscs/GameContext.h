#pragma once
#include "GameContext/DigimonContext.h"
#include "GameContext/Inventory.h"
#include "GameContext/Settings.h"
#include "GameContext/Player.h"
#include "GameContext/DigiFarm.h"
#include "GameContext/Quest.h"
#include "GameContext/Digiline.h"
#include "GameContext/OnlineBattleBox.h"
#include "GameContext/UserStats.h"

#include <stdint.h>

#include <map>
#include <vector>

typedef uint8_t undefined;

namespace dscs
{
    struct SeenData
    {
        uint32_t entryId;
        uint16_t seenState;
        uint16_t field2_0x6;
    };

    enum class StoryModeEnum
    {
        CYBER_SLEUTH   = 0,
        HACKERS_MEMORY = 1
    };

    struct StoryMode
    {
    private:
        void** methods;

    public:
        StoryModeEnum mode;
        uint32_t pad;
    };

    struct Workspace
    {
    private:
        void** destroy;

    public:
        uint32_t flags[100];
    };

    struct FlagArray
    {
        void** destroy;
        uint32_t flags[626];

        bool get(int32_t flag) { return (flags[flag >> 5] >> (flag & 0x1F) & 1) != 0; }

        void clear(int32_t flag) { flags[flag >> 5] &= ~(1 << (flag & 0x1F)); }

        // not equivalent to Flag.Set, since it doesn't call the achievement check
        void set(int32_t flag) { flags[flag >> 5] |= (1 << (flag & 0x1F)); }
    };

    struct GameContext
    {
    private:
        void** methods;

    public:
        FlagArray* flags;
        DigimonContext* digimonCS;
        DigiFarm* digiFarmCS;
        Inventory* inventoryCS;
        Workspace* work;
        PlayerStruct* playerCS;
        QuestsStruct* questsCS;
        DigilineStruct* digilineCS;
        Settings* settings;
        UserStats* stats;
        OnlineBattleBox* battleBox;
        DigimonContext* digimonHM;
        DigiFarm* digiFarmHM;
        Inventory* inventoryHM;
        PlayerStruct* playerHM;
        QuestsStruct* questsHM;
        DigilineStruct* digilineHM;
        StoryMode* storyMode;
        struct GameContext_0x98* unk0x98;
        struct GameContext_0xa0* unk0xa0;
        struct GameContext_0xa8* unk0xa8;
        struct DLCFlags* dlcflags;
        struct GameContext_0xB8* unk0xb8;
        uint64_t** unk0xc0;
    };
} // namespace dscs