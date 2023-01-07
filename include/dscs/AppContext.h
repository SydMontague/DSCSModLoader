#pragma once
#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct RootContextFuncs
    {
        undefined* destroy;
        undefined* startup;
        undefined* shutdown;
        undefined* update;
        undefined* field4_0x20;
        undefined* field5_0x28;
    };

    struct ResolutionEntry
    {
        int32_t resolutionId;
        int32_t width;
        int32_t height;
    };

    struct FontEntry
    {
        int32_t id;
        char field1_0x4[60];
        float unkFactor;
    };

    struct FontStruct
    {
        ResolutionEntry resolution;
        int32_t renderWidth;
        int32_t renderHeight;
        int32_t offsetX;
        int32_t offsetY;
        undefined field5_0x1c;
        undefined field6_0x1d;
        undefined field7_0x1e;
        undefined field8_0x1f;
        undefined field9_0x20;
        undefined field10_0x21;
        undefined field11_0x22;
        undefined field12_0x23;
        float viewportWidth;
        float viewportHeight;
        int32_t resolutionBaseValue;
        int32_t orientation; // ?
        int32_t field17_0x34;
        int32_t field18_0x38;
        int32_t field19_0x3c;
        undefined field20_0x40;
        undefined field21_0x41;
        undefined field22_0x42;
        undefined field23_0x43;
        undefined field24_0x44;
        undefined field25_0x45;
        undefined field26_0x46;
        undefined field27_0x47;
        bool field28_0x48;
        undefined field29_0x49;
        bool field30_0x4a;
        undefined field31_0x4b;
        undefined field32_0x4c;
        undefined field33_0x4d;
        undefined field34_0x4e;
        undefined field35_0x4f;
        int32_t field36_0x50;
        undefined field37_0x54;
        undefined field38_0x55;
        undefined field39_0x56;
        undefined field40_0x57;
        undefined field41_0x58;
        undefined field42_0x59;
        undefined field43_0x5a;
        undefined field44_0x5b;
        FontEntry fontEntries[8];
        int32_t fontCount;
        double updateBegin;
        double updateEnd;
        undefined field49_0x290;
        undefined field50_0x291;
        undefined field51_0x292;
        undefined field52_0x293;
        undefined field53_0x294;
        undefined field54_0x295;
        undefined field55_0x296;
        undefined field56_0x297;
    };

    struct AppContext
    {
        struct RootContextFuncs* vftable;
        double timer;
        float timeDelta;
        bool field3_0x14;
        undefined field4_0x15;
        undefined field5_0x16;
        undefined field6_0x17;
        struct FontStruct field7_0x18;
    };

} // namespace dscs