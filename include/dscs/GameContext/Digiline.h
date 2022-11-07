#pragma once

#include <stdint.h>

#include <map>
#include <vector>

typedef uint8_t undefined;

namespace dscs
{
    struct DigilineEntry_sub
    {
        int8_t field0_0x0;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        int32_t field4_0x4;
        int32_t field5_0x8;
        int8_t string[80];
        int8_t string2[152];
    };

    struct DigilineEntry
    {
        int8_t field0_0x0;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        undefined field4_0x4;
        undefined field5_0x5;
        undefined field6_0x6;
        undefined field7_0x7;
        uint32_t field8_0x8;
        int32_t field9_0xc;
        int32_t field10_0x10;
        float field11_0x14;
        int32_t field12_0x18;
        DigilineEntry_sub field13_0x1c[3];
    };

    struct DigilineEntry2
    {
        int8_t field0_0x0;
        undefined field1_0x1;
        undefined field2_0x2;
        undefined field3_0x3;
        uint32_t field4_0x4;
        int8_t field5_0x8;
        undefined field6_0x9;
        undefined field7_0xa;
        undefined field8_0xb;
        int8_t field9_0xc[464];
        undefined field10_0x1dc;
        undefined field11_0x1dd;
        undefined field12_0x1de;
        undefined field13_0x1df;
        int8_t field14_0x1e0[8];
        int32_t field15_0x1e8;
        int8_t field16_0x1ec[72];
    };

    struct DigilineStruct
    {
    private:
        void** destroy;

    public:
        std::vector<DigilineEntry> unk1;
        std::vector<DigilineEntry> unk2;
        std::vector<DigilineEntry> unk3;
        std::vector<uint32_t> field4_0x50;
        std::vector<DigilineEntry2> field5_0x68;
        undefined field6_0x80;
        undefined field7_0x81;
        undefined field8_0x82;
        undefined field9_0x83;
        undefined field10_0x84;
        undefined field11_0x85;
        undefined field12_0x86;
        undefined field13_0x87;
    };

} // namespace dscs