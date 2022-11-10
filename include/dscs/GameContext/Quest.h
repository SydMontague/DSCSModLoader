#pragma once
#include <stdint.h>

#include <map>

typedef uint8_t undefined;

namespace dscs
{
    struct quest_para_sub2
    {
        uint32_t id;
        uint8_t field1_0x4;
        undefined field2_0x5;
        undefined field3_0x6;
        undefined field4_0x7;
    };

    struct quest_para_sub1
    {
        uint32_t id;
        uint32_t field1_0x4;
        uint32_t field2_0x8;
        uint32_t field3_0xc;
        uint32_t field4_0x10;
        uint32_t field5_0x14;
    };

    struct quest_para_sub3 {

        int32_t field4_0x38;
        int32_t field5_0x3c;
        int32_t field6_0x40;
        int32_t field7_0x44;
        int32_t field8_0x48;
        int32_t field9_0x4c;
        int32_t field10_0x50;
        int32_t field11_0x54;
        int32_t field12_0x58;
        int32_t field13_0x5c;
        int32_t field14_0x60;
        int32_t field15_0x64;
        int32_t field16_0x68;
        int32_t field17_0x6c;
        int32_t field18_0x70;
        int32_t field19_0x74;
    };

    struct QuestsStruct
    {
        void** destroy;
        std::map<int, quest_para_sub1> unk1;
        std::map<int, quest_para_sub2> unk2;
        std::map<int, quest_para_sub2> unk3;
        quest_para_sub3 unk4;
        uint32_t game;
        int32_t field21_0x7c;
    };

} // namespace dscs