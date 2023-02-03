#pragma once
#include <stdint.h>

#include <vector>
typedef uint8_t undefined;

namespace dscs
{
    struct FVector4
    {
        float x;
        float y;
        float z;
        float w;
    };

    struct PlayerData
    {
        uint32_t playerSex;
        char name[40];
        uint32_t controlPlayerType;
        double playtime;
        uint32_t money;
        uint32_t hackerPoints;
        int32_t hackerLevel;
        int32_t maxPartyMemory;
        int32_t storyProgress;
        int32_t clearNum;
        int32_t mapId;
        int32_t submapId;
        undefined field64_0x58;
        undefined field65_0x59;
        undefined field66_0x5a;
        undefined field67_0x5b;
        undefined field68_0x5c;
        undefined field69_0x5d;
        undefined field70_0x5e;
        undefined field71_0x5f;
        FVector4 position;
        FVector4 rotation;
        float cameraZoom;
        undefined field108_0x84;
        undefined field109_0x85;
        undefined field110_0x86;
        undefined field111_0x87;
        undefined field112_0x88;
        undefined field113_0x89;
        undefined field114_0x8a;
        undefined field115_0x8b;
        FVector4 digiPosition[4];
        FVector4 digiRotation[4];
        int32_t currentArea;
        int32_t unkAreaId;
        int32_t field252_0x114;
        FVector4 field256_0x118;
        undefined field272_0x128;
        undefined field273_0x129;
        undefined field274_0x12a;
        undefined field275_0x12b;
        undefined field276_0x12c;
        undefined field277_0x12d;
        undefined field278_0x12e;
        undefined field279_0x12f;
        undefined field280_0x130;
        undefined field281_0x131;
        undefined field282_0x132;
        undefined field283_0x133;
        undefined field284_0x134;
        undefined field285_0x135;
        undefined field286_0x136;
        undefined field287_0x137;
        undefined field288_0x138;
        undefined field289_0x139;
        undefined field290_0x13a;
        undefined field291_0x13b;
        undefined field292_0x13c;
        undefined field293_0x13d;
        undefined field294_0x13e;
        undefined field295_0x13f;
        undefined field296_0x140;
        undefined field297_0x141;
        undefined field298_0x142;
        undefined field299_0x143;
        FVector4 field300_0x144[4];
        FVector4 field368_0x184[4];
        int32_t fieldGuest;
        char clothModelName[8];
        double field430_0x1d0;
        uint8_t field431_0x1d8[248];
    };

    struct LearnedSkill
    {
        bool isEnabled;
        char field1_0x1;
        uint16_t field2_0x2;
        uint32_t skillId;
        uint32_t unk6;
    };
    struct PlayerStruct
    {
        void* field0_0x0;
        struct PlayerData data;
        struct std::vector<LearnedSkill*> learnedSkills;
    };

    static_assert(sizeof(PlayerData) == 0x2d0);
    static_assert(sizeof(PlayerStruct) == 0x2f0);
    static_assert(sizeof(LearnedSkill) == 0xc);
} // namespace dscs