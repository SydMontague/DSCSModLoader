#pragma once
#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct UserStatsData
    {
        uint32_t field0_0x0;
        uint32_t field1_0x4;
        uint32_t field2_0x8;
        uint32_t field3_0xc;
        uint32_t field4_0x10;
        uint32_t maxABICount; /* Created by retype action */
        uint32_t field6_0x18;
        uint32_t field7_0x1c;
        uint32_t field8_0x20;
        uint32_t field9_0x24;
        uint32_t field10_0x28;
        uint32_t field11_0x2c;
        uint32_t field12_0x30;
        uint32_t field13_0x34;
        uint32_t field14_0x38;
        uint32_t field15_0x3c;
        uint32_t field16_0x40;
        uint32_t field17_0x44;
        uint32_t field18_0x48;
        uint32_t field19_0x4c;
        uint32_t field20_0x50;
        uint32_t field21_0x54;
        uint32_t field22_0x58;
        uint32_t field23_0x5c;
        uint32_t field24_0x60;
        uint32_t field25_0x64;
        uint32_t field26_0x68;
        uint32_t field27_0x6c;
        uint32_t field28_0x70;
        uint32_t field29_0x74;
        uint32_t field30_0x78;
        uint32_t field31_0x7c;
        uint32_t field32_0x80;
        uint32_t field33_0x84;
        uint32_t field34_0x88;
        uint32_t field35_0x8c;
        uint32_t field36_0x90;
        uint32_t field37_0x94;
        uint32_t field38_0x98;
        uint32_t field39_0x9c;
        uint32_t field40_0xa0;
        uint32_t field41_0xa4;
        uint32_t field42_0xa8;
        uint32_t field43_0xac;
        uint32_t field44_0xb0;
        uint32_t field45_0xb4;
        uint32_t field46_0xb8;
        uint32_t field47_0xbc;
        uint32_t field48_0xc0;
        uint32_t field49_0xc4;
        uint32_t field50_0xc8;
        uint32_t field51_0xcc;
        uint32_t field52_0xd0;
        uint32_t field53_0xd4;
        uint32_t field54_0xd8;
        uint32_t field55_0xdc;
        uint32_t field56_0xe0;
        uint32_t field57_0xe4;
        uint32_t field58_0xe8;
        uint32_t field59_0xec;
        uint32_t field60_0xf0;
        uint32_t field61_0xf4;
        uint32_t field62_0xf8;
        uint32_t field63_0xfc;
        uint32_t field64_0x100;
        uint32_t field65_0x104;
        uint32_t field66_0x108;
        uint32_t field67_0x10c;
        uint32_t field68_0x110;
        uint32_t field69_0x114;
        uint32_t field70_0x118;
        uint32_t field71_0x11c;
        uint32_t field72_0x120;
        uint32_t field73_0x124;
        uint32_t field74_0x128;
        uint32_t field75_0x12c;
        uint32_t field76_0x130;
        uint32_t field77_0x134;
        uint32_t field78_0x138;
        uint32_t field79_0x13c;
        uint32_t field80_0x140;
        uint32_t field81_0x144;
        uint32_t field82_0x148;
        uint32_t field83_0x14c;
        uint32_t field84_0x150;
        uint32_t field85_0x154;
        uint32_t field86_0x158;
        uint32_t field87_0x15c;
        uint32_t field88_0x160;
        uint32_t field89_0x164;
        uint32_t field90_0x168;
        uint32_t field91_0x16c;
        uint32_t field92_0x170;
        uint32_t field93_0x174;
        uint32_t field94_0x178;
    };

    struct UserStats
    {
        void** destroy;
        struct UserStatsData data;
        undefined pad[4];
    };

} // namespace dscs