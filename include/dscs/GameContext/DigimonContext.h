#pragma once

#include <stdint.h>

#include <map>
#include <vector>

typedef uint8_t undefined;

namespace dscs
{
    // forward declaration
    struct BankSaveEntry;
    struct FarmSaveEntry;

    struct DigimonMove
    {
        uint32_t type;
        uint32_t move;
    };

    struct ScanData
    {
        uint16_t digimonId;
        uint16_t scanrate;
    };

    struct Digimon
    {
        uint32_t uniqueId;
        uint32_t digimonId;
        uint32_t element;
        uint32_t type;
        uint32_t stage;
        char name[72];
        int16_t memoryCost;
        int16_t field19_0x5e;
        int16_t sortType;
        int16_t field23_0x62;
        int16_t sortGeneration;
        int16_t sortName;
        int16_t level;
        int16_t levelCapBonus;
        int16_t levelCapBase;
        int16_t field32_0x6e;
        uint32_t exp;
        int32_t growthType;
        uint32_t personality;
        uint32_t currentHP;
        int32_t baseHP;
        uint16_t trainedHP;
        uint16_t field43_0x86;
        uint32_t currentSP;
        uint32_t baseSP;
        uint16_t trainedSP;
        uint16_t baseOff;
        uint16_t trainedATK;
        uint16_t baseDef;
        uint16_t trainedDef;
        uint16_t baseInt;
        uint16_t trainedInt;
        uint16_t baseSpd;
        uint16_t trainedSpd;
        uint16_t abi;
        uint16_t cam;
        uint16_t field58_0xa6;
        DigimonMove move[6];
        DigimonMove learnedMoves[20];
        DigimonMove overflowMoves[20];
        int32_t supportSkill;
        int16_t numEquipSlots;
        int16_t equip[3];
        int16_t accessory;
        int16_t leaderOfFarm;
        int16_t totalTrainedHP;
        int16_t totalTrainedSP;
        int16_t totalTrainedATK;
        int16_t totalTrainedDEF;
        int16_t totalTrainedINT;
        int16_t totalTrainedSPD;
    };

    struct PartyEntry
    {
        bool isFilled;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        uint32_t field4_0x4;
        uint32_t field5_0x8;
        uint32_t field6_0xc;
        Digimon* digimonPtr;

        PartyEntry& operator=(const BankSaveEntry& entry);

        PartyEntry& operator=(const BankSaveEntry* entry);

        PartyEntry& operator=(const FarmSaveEntry& entry);

        PartyEntry& operator=(const FarmSaveEntry* entry);
    };

    struct DigimonContext
    {
        void** functions;
        std::map<int, ScanData*> scanData;
        int32_t bankSize;
        undefined pad1[4];
        std::vector<PartyEntry*> bank;
        std::vector<PartyEntry*> farm[5];
        std::vector<PartyEntry*> party;
        std::vector<PartyEntry*> guestDigimon;
        int32_t story;
        undefined pad2[4];
        std::vector<PartyEntry*> partyBackup;
        std::vector<PartyEntry*> partyBackup2;
    };

    static_assert(sizeof(Digimon) == 0x234);
    static_assert(sizeof(PartyEntry) == 0x18);
    static_assert(sizeof(DigimonMove) == 0x08);
    static_assert(sizeof(ScanData) == 0x04);
    static_assert(sizeof(DigimonContext) == 0x118);
} // namespace dscs