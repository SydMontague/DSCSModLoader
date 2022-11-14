#pragma once
#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct DigiFarmEntry
    {
        bool isUnlocked;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        int32_t farmId;
        int32_t level;
        int32_t items[5];
        int32_t activity;
        float xpTimer;
        float trainGainTimer;
        float trainTimer;
        int32_t trainActive;
        int32_t trainIntensity;
        float developTimer;
        int32_t developValue;
        int32_t developActive;
        int32_t developCourse;
        int32_t developPointTable[7];
        int32_t developItem;
        int32_t developAmount;
        float investigateTimer;
        int32_t investigateValue;
        int32_t investigateActive;
        int32_t investigatePointTable[7];
        int32_t investigateItem;
        int32_t investigateAmount;
        int32_t unk1;
        int32_t unk2;
        int32_t unk3;
    };

    struct DigiFarmData
    {
        struct DigiFarmEntry farms[5];
        undefined field1_0x348;
        undefined field2_0x349;
        undefined field3_0x34a;
        undefined field4_0x34b;
        undefined field5_0x34c;
        undefined field6_0x34d;
        undefined field7_0x34e;
        undefined field8_0x34f;
        undefined field9_0x350;
        undefined field10_0x351;
        undefined field11_0x352;
        undefined field12_0x353;
    };

    struct DigiFarm
    {
        void** destroy;
        bool farmRunning;
        undefined field2_0x9;
        undefined field3_0xa;
        undefined field4_0xb;
        struct DigiFarmData data;
    };
} // namespace dscs