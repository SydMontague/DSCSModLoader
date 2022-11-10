#pragma once
#include "GameContext.h"

#include <stdint.h>

#include <map>
#include <vector>

typedef uint8_t undefined;

namespace dscs
{

    struct FarmSaveEntry
    {
        bool isFilled;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        uint32_t field4_0x4;
        uint32_t field5_0x8;
        uint32_t field6_0xc;
        Digimon digimonPtr;

        FarmSaveEntry& operator=(const PartyEntry& entry)
        {
            this->isFilled = entry.isFilled;
            this->field4_0x4 = entry.field4_0x4;
            this->field5_0x8 = entry.field5_0x8;
            this->field6_0xc = entry.field6_0xc;
            this->digimonPtr = *entry.digimonPtr;
            return *this;
        }

        FarmSaveEntry& operator=(const PartyEntry* entry)
        {
            this->isFilled = entry->isFilled;
            this->field4_0x4 = entry->field4_0x4;
            this->field5_0x8 = entry->field5_0x8;
            this->field6_0xc = entry->field6_0xc;
            this->digimonPtr = *entry->digimonPtr;
            return *this;
        }
    };

    struct BankSaveEntry
    {
        bool isFilled;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        uint32_t field4_0x4;
        uint32_t field5_0x8;
        Digimon digimonPtr;

        BankSaveEntry& operator=(const PartyEntry& entry)
        {
            this->isFilled = entry.isFilled;
            this->field4_0x4 = entry.field4_0x4;
            this->field5_0x8 = entry.field5_0x8;
            this->digimonPtr = *entry.digimonPtr;
            return *this;
        }

        BankSaveEntry& operator=(const PartyEntry* entry)
        {
            this->isFilled = entry->isFilled;
            this->field4_0x4 = entry->field4_0x4;
            this->field5_0x8 = entry->field5_0x8;
            this->digimonPtr = *entry->digimonPtr;
            return *this;
        }
    };

    struct SaveBattleBox
    {
        uint8_t field0_0x0;
        char name[40];
        undefined field2_0x29;
        undefined field3_0x2a;
        undefined field4_0x2b;
        BankSaveEntry party[11];
    };

    struct StorySave
    {
        ScanData scanData[400];
        BankSaveEntry bank[300];
        uint32_t bankSize;
        FarmSaveEntry farm[5][10];
        BankSaveEntry party[11];
        BankSaveEntry guest[2];
        GameContext0x18_data context_0x18;
        InventoryItem inventoryBag[2000];
        InventoryItem inventoryUnk1[100];
        InventoryItem inventoryUnk2[100];
        undefined pad1[16];
        PlayerData player;
        LearnedSkill hackingSkills[30];
        undefined pad2[8];
        quest_para_sub1 questUnk1[350];
        quest_para_sub2 questUnk2[350];
        quest_para_sub2 questUnk3[350];
        quest_para_sub3 questUnk4;
        DigilineEntry digiline1[30];
        DigilineEntry digiline2[30];
        DigilineEntry digiline3[30];
        uint32_t digilineUnk[90];
        DigilineEntry2 digiline4[90];
        undefined pad3[16];
    };

    struct Savegame
    {
        uint32_t version;
        uint32_t flags[626];
        SeenData seenData[400];
        uint32_t work[100];
        SettingsData settings;
        UserStatsData stats;
        OnlineBattleBox_sub1 battleBoxData;
        SaveBattleBox battleBoxes[6];
        StorySave saveCS;
        StorySave saveHM;
    };

} // namespace dscs