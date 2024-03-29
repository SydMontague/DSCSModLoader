#pragma once
#include "DigimonContext.h"

#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct OnlineBattleBoxEntry
    {
        uint8_t field0_0x0;
        char name[40];
        undefined field14_0x29;
        undefined field15_0x2a;
        undefined field16_0x2b;
        undefined field17_0x2c;
        undefined field18_0x2d;
        undefined field19_0x2e;
        undefined field20_0x2f;
        PartyEntry party[11];
    };

    struct OnlineBattleBox_sub1
    {
        uint32_t field1_0x8;
        uint32_t field2_0xc;
        undefined field3_0x10;
        undefined field4_0x11;
        undefined field5_0x12;
        undefined field6_0x13;
        undefined field7_0x14;
        undefined field8_0x15;
        undefined field9_0x16;
        undefined field10_0x17;
        uint32_t field11_0x18;
        uint32_t field12_0x1c;
        int8_t field13_0x20[128];
        uint32_t field14_0xa0;
        uint32_t field15_0xa4;
        uint32_t field16_0xa8;
        uint32_t field17_0xac;
        uint32_t field18_0xb0;
    };

    struct OnlineBattleBox
    {
        void** destroy;
        OnlineBattleBox_sub1 data;
        undefined field19_0xb4;
        undefined field20_0xb5;
        undefined field21_0xb6;
        undefined field22_0xb7;
        OnlineBattleBoxEntry boxes[6];
        undefined field24_0x808;
        undefined field25_0x809;
        undefined field26_0x80a;
        undefined field27_0x80b;
        undefined field28_0x80c;
        undefined field29_0x80d;
        undefined field30_0x80e;
        undefined field31_0x80f;
        undefined field32_0x810;
        undefined field33_0x811;
        undefined field34_0x812;
        undefined field35_0x813;
        undefined field36_0x814;
        undefined field37_0x815;
        undefined field38_0x816;
        undefined field39_0x817;
        uint16_t field40_0x818;
        int8_t field41_0x81a;
        undefined field42_0x81b;
        undefined field43_0x81c;
        undefined field44_0x81d;
        undefined field45_0x81e;
        undefined field46_0x81f;
        undefined field47_0x820;
        undefined field48_0x821;
        undefined field49_0x822;
        undefined field50_0x823;
        undefined field51_0x824;
        undefined field52_0x825;
        undefined field53_0x826;
        undefined field54_0x827;
        undefined field55_0x828;
        undefined field56_0x829;
        undefined field57_0x82a;
        undefined field58_0x82b;
        undefined field59_0x82c;
        undefined field60_0x82d;
        undefined field61_0x82e;
        undefined field62_0x82f;
        undefined field63_0x830;
        undefined field64_0x831;
        undefined field65_0x832;
        undefined field66_0x833;
        undefined field67_0x834;
        undefined field68_0x835;
        undefined field69_0x836;
        undefined field70_0x837;
        undefined field71_0x838;
        undefined field72_0x839;
        undefined field73_0x83a;
        undefined field74_0x83b;
        undefined field75_0x83c;
        undefined field76_0x83d;
        undefined field77_0x83e;
        undefined field78_0x83f;
        undefined field79_0x840;
        undefined field80_0x841;
        undefined field81_0x842;
        undefined field82_0x843;
        undefined field83_0x844;
        undefined field84_0x845;
        undefined field85_0x846;
        undefined field86_0x847;
        undefined field87_0x848;
        undefined field88_0x849;
        undefined field89_0x84a;
        undefined field90_0x84b;
        undefined field91_0x84c;
        undefined field92_0x84d;
        undefined field93_0x84e;
        undefined field94_0x84f;
    };
} // namespace dscs