#pragma once
#include <stdint.h>

#include <map>
#include <vector>

typedef uint8_t undefined;

namespace dscs
{
    enum class StoryModeEnum
    {
        CYBER_SLEUTH   = 0,
        HACKERS_MEMORY = 1
    };

    struct PartyEntry
    {
        bool isFilled;
        undefined field_0x1;
        undefined field_0x2;
        undefined field_0x3;
        undefined field_0x4;
        undefined field_0x5;
        undefined field_0x6;
        undefined field_0x7;
        undefined field_0x8;
        undefined field_0x9;
        undefined field_0xa;
        undefined field_0xb;
        undefined field_0xc;
        undefined field_0xd;
        undefined field_0xe;
        undefined field_0xf;
        struct Digimon* digimonPtr;
        undefined field_0x18;
        undefined field_0x19;
        undefined field_0x1a;
        undefined field_0x1b;
        undefined field_0x1c;
        undefined field_0x1d;
        undefined field_0x1e;
        undefined field_0x1f;
    };

    struct InventoryItem
    {
        bool isFilled;
        char pad[3];
        int32_t slotIndex;
        uint32_t bag;
        uint32_t id;
        int32_t sortValue;
        uint32_t quantity;
    };

    struct LearnedSkill
    {
        bool isEnabled;
        char field_0x1;
        uint16_t field_0x2;
        uint32_t skillId;
        uint32_t field_0x8;
        uint32_t field_0xc;
        uint32_t field_0x10;
        uint32_t field_0x14;
        uint32_t field_0x18;
        uint32_t field_0x1c;
    };

    struct DigimonMove
    {
        uint32_t type;
        uint32_t move;
    };

    struct Digimon
    {
        undefined field_0x0;
        undefined field_0x1;
        undefined field_0x2;
        undefined field_0x3;
        uint32_t digimonId;
        undefined field_0x8;
        undefined field_0x9;
        undefined field_0xa;
        undefined field_0xb;
        undefined field_0xc;
        undefined field_0xd;
        undefined field_0xe;
        undefined field_0xf;
        undefined field_0x10;
        undefined field_0x11;
        undefined field_0x12;
        undefined field_0x13;
        char name[72];
        int16_t memoryCost;
        undefined field_0x5e;
        undefined field_0x5f;
        undefined field_0x60;
        undefined field_0x61;
        undefined field_0x62;
        undefined field_0x63;
        undefined field_0x64;
        undefined field_0x65;
        undefined field_0x66;
        undefined field_0x67;
        int16_t level;
        int16_t levelCapBonus;
        int16_t levelCapBase;
        undefined field_0x6e;
        undefined field_0x6f;
        uint32_t exp;
        undefined field_0x74;
        undefined field_0x75;
        undefined field_0x76;
        undefined field_0x77;
        uint32_t personality;
        uint32_t currentHP;
        int32_t baseHP;
        uint16_t trainedHP;
        undefined field_0x86;
        undefined field_0x87;
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
        undefined field_0xa6;
        undefined field_0xa7;
        DigimonMove move1;
        DigimonMove move2;
        DigimonMove move3;
        DigimonMove move4;
        DigimonMove move5;
        DigimonMove move6;
        undefined field_0xd8;
        undefined field_0xd9;
        undefined field_0xda;
        undefined field_0xdb;
        undefined field_0xdc;
        undefined field_0xdd;
        undefined field_0xde;
        undefined field_0xdf;
        undefined field_0xe0;
        undefined field_0xe1;
        undefined field_0xe2;
        undefined field_0xe3;
        undefined field_0xe4;
        undefined field_0xe5;
        undefined field_0xe6;
        undefined field_0xe7;
        undefined field_0xe8;
        undefined field_0xe9;
        undefined field_0xea;
        undefined field_0xeb;
        undefined field_0xec;
        undefined field_0xed;
        undefined field_0xee;
        undefined field_0xef;
        undefined field_0xf0;
        undefined field_0xf1;
        undefined field_0xf2;
        undefined field_0xf3;
        undefined field_0xf4;
        undefined field_0xf5;
        undefined field_0xf6;
        undefined field_0xf7;
        undefined field_0xf8;
        undefined field_0xf9;
        undefined field_0xfa;
        undefined field_0xfb;
        undefined field_0xfc;
        undefined field_0xfd;
        undefined field_0xfe;
        undefined field_0xff;
        undefined field_0x100;
        undefined field_0x101;
        undefined field_0x102;
        undefined field_0x103;
        undefined field_0x104;
        undefined field_0x105;
        undefined field_0x106;
        undefined field_0x107;
        undefined field_0x108;
        undefined field_0x109;
        undefined field_0x10a;
        undefined field_0x10b;
        undefined field_0x10c;
        undefined field_0x10d;
        undefined field_0x10e;
        undefined field_0x10f;
        undefined field_0x110;
        undefined field_0x111;
        undefined field_0x112;
        undefined field_0x113;
        undefined field_0x114;
        undefined field_0x115;
        undefined field_0x116;
        undefined field_0x117;
        undefined field_0x118;
        undefined field_0x119;
        undefined field_0x11a;
        undefined field_0x11b;
        undefined field_0x11c;
        undefined field_0x11d;
        undefined field_0x11e;
        undefined field_0x11f;
        undefined field_0x120;
        undefined field_0x121;
        undefined field_0x122;
        undefined field_0x123;
        undefined field_0x124;
        undefined field_0x125;
        undefined field_0x126;
        undefined field_0x127;
        undefined field_0x128;
        undefined field_0x129;
        undefined field_0x12a;
        undefined field_0x12b;
        undefined field_0x12c;
        undefined field_0x12d;
        undefined field_0x12e;
        undefined field_0x12f;
        undefined field_0x130;
        undefined field_0x131;
        undefined field_0x132;
        undefined field_0x133;
        undefined field_0x134;
        undefined field_0x135;
        undefined field_0x136;
        undefined field_0x137;
        undefined field_0x138;
        undefined field_0x139;
        undefined field_0x13a;
        undefined field_0x13b;
        undefined field_0x13c;
        undefined field_0x13d;
        undefined field_0x13e;
        undefined field_0x13f;
        undefined field_0x140;
        undefined field_0x141;
        undefined field_0x142;
        undefined field_0x143;
        undefined field_0x144;
        undefined field_0x145;
        undefined field_0x146;
        undefined field_0x147;
        undefined field_0x148;
        undefined field_0x149;
        undefined field_0x14a;
        undefined field_0x14b;
        undefined field_0x14c;
        undefined field_0x14d;
        undefined field_0x14e;
        undefined field_0x14f;
        undefined field_0x150;
        undefined field_0x151;
        undefined field_0x152;
        undefined field_0x153;
        undefined field_0x154;
        undefined field_0x155;
        undefined field_0x156;
        undefined field_0x157;
        undefined field_0x158;
        undefined field_0x159;
        undefined field_0x15a;
        undefined field_0x15b;
        undefined field_0x15c;
        undefined field_0x15d;
        undefined field_0x15e;
        undefined field_0x15f;
        undefined field_0x160;
        undefined field_0x161;
        undefined field_0x162;
        undefined field_0x163;
        undefined field_0x164;
        undefined field_0x165;
        undefined field_0x166;
        undefined field_0x167;
        undefined field_0x168;
        undefined field_0x169;
        undefined field_0x16a;
        undefined field_0x16b;
        undefined field_0x16c;
        undefined field_0x16d;
        undefined field_0x16e;
        undefined field_0x16f;
        undefined field_0x170;
        undefined field_0x171;
        undefined field_0x172;
        undefined field_0x173;
        undefined field_0x174;
        undefined field_0x175;
        undefined field_0x176;
        undefined field_0x177;
        undefined field_0x178;
        undefined field_0x179;
        undefined field_0x17a;
        undefined field_0x17b;
        undefined field_0x17c;
        undefined field_0x17d;
        undefined field_0x17e;
        undefined field_0x17f;
        undefined field_0x180;
        undefined field_0x181;
        undefined field_0x182;
        undefined field_0x183;
        undefined field_0x184;
        undefined field_0x185;
        undefined field_0x186;
        undefined field_0x187;
        undefined field_0x188;
        undefined field_0x189;
        undefined field_0x18a;
        undefined field_0x18b;
        undefined field_0x18c;
        undefined field_0x18d;
        undefined field_0x18e;
        undefined field_0x18f;
        undefined field_0x190;
        undefined field_0x191;
        undefined field_0x192;
        undefined field_0x193;
        undefined field_0x194;
        undefined field_0x195;
        undefined field_0x196;
        undefined field_0x197;
        undefined field_0x198;
        undefined field_0x199;
        undefined field_0x19a;
        undefined field_0x19b;
        undefined field_0x19c;
        undefined field_0x19d;
        undefined field_0x19e;
        undefined field_0x19f;
        undefined field_0x1a0;
        undefined field_0x1a1;
        undefined field_0x1a2;
        undefined field_0x1a3;
        undefined field_0x1a4;
        undefined field_0x1a5;
        undefined field_0x1a6;
        undefined field_0x1a7;
        undefined field_0x1a8;
        undefined field_0x1a9;
        undefined field_0x1aa;
        undefined field_0x1ab;
        undefined field_0x1ac;
        undefined field_0x1ad;
        undefined field_0x1ae;
        undefined field_0x1af;
        undefined field_0x1b0;
        undefined field_0x1b1;
        undefined field_0x1b2;
        undefined field_0x1b3;
        undefined field_0x1b4;
        undefined field_0x1b5;
        undefined field_0x1b6;
        undefined field_0x1b7;
        undefined field_0x1b8;
        undefined field_0x1b9;
        undefined field_0x1ba;
        undefined field_0x1bb;
        undefined field_0x1bc;
        undefined field_0x1bd;
        undefined field_0x1be;
        undefined field_0x1bf;
        undefined field_0x1c0;
        undefined field_0x1c1;
        undefined field_0x1c2;
        undefined field_0x1c3;
        undefined field_0x1c4;
        undefined field_0x1c5;
        undefined field_0x1c6;
        undefined field_0x1c7;
        undefined field_0x1c8;
        undefined field_0x1c9;
        undefined field_0x1ca;
        undefined field_0x1cb;
        undefined field_0x1cc;
        undefined field_0x1cd;
        undefined field_0x1ce;
        undefined field_0x1cf;
        undefined field_0x1d0;
        undefined field_0x1d1;
        undefined field_0x1d2;
        undefined field_0x1d3;
        undefined field_0x1d4;
        undefined field_0x1d5;
        undefined field_0x1d6;
        undefined field_0x1d7;
        undefined field_0x1d8;
        undefined field_0x1d9;
        undefined field_0x1da;
        undefined field_0x1db;
        undefined field_0x1dc;
        undefined field_0x1dd;
        undefined field_0x1de;
        undefined field_0x1df;
        undefined field_0x1e0;
        undefined field_0x1e1;
        undefined field_0x1e2;
        undefined field_0x1e3;
        undefined field_0x1e4;
        undefined field_0x1e5;
        undefined field_0x1e6;
        undefined field_0x1e7;
        undefined field_0x1e8;
        undefined field_0x1e9;
        undefined field_0x1ea;
        undefined field_0x1eb;
        undefined field_0x1ec;
        undefined field_0x1ed;
        undefined field_0x1ee;
        undefined field_0x1ef;
        undefined field_0x1f0;
        undefined field_0x1f1;
        undefined field_0x1f2;
        undefined field_0x1f3;
        undefined field_0x1f4;
        undefined field_0x1f5;
        undefined field_0x1f6;
        undefined field_0x1f7;
        undefined field_0x1f8;
        undefined field_0x1f9;
        undefined field_0x1fa;
        undefined field_0x1fb;
        undefined field_0x1fc;
        undefined field_0x1fd;
        undefined field_0x1fe;
        undefined field_0x1ff;
        undefined field_0x200;
        undefined field_0x201;
        undefined field_0x202;
        undefined field_0x203;
        undefined field_0x204;
        undefined field_0x205;
        undefined field_0x206;
        undefined field_0x207;
        undefined field_0x208;
        undefined field_0x209;
        undefined field_0x20a;
        undefined field_0x20b;
        undefined field_0x20c;
        undefined field_0x20d;
        undefined field_0x20e;
        undefined field_0x20f;
        undefined field_0x210;
        undefined field_0x211;
        undefined field_0x212;
        undefined field_0x213;
        undefined field_0x214;
        undefined field_0x215;
        undefined field_0x216;
        undefined field_0x217;
        undefined field_0x218;
        undefined field_0x219;
        undefined field_0x21a;
        undefined field_0x21b;
        short numEquipSlots;
        short equip[3];
        short accessory;
        undefined field_0x226;
        undefined field_0x227;
        undefined field_0x228;
        undefined field_0x229;
        undefined field_0x22a;
        undefined field_0x22b;
        undefined field_0x22c;
        undefined field_0x22d;
        undefined field_0x22e;
        undefined field_0x22f;
        undefined field_0x230;
        undefined field_0x231;
        undefined field_0x232;
        undefined field_0x233;
        undefined field_0x234;
        undefined field_0x235;
        undefined field_0x236;
        undefined field_0x237;
        undefined field_0x238;
        undefined field_0x239;
        undefined field_0x23a;
        undefined field_0x23b;
        undefined field_0x23c;
        undefined field_0x23d;
        undefined field_0x23e;
        undefined field_0x23f;
        undefined field_0x240;
        undefined field_0x241;
        undefined field_0x242;
        undefined field_0x243;
        undefined field_0x244;
        undefined field_0x245;
        undefined field_0x246;
        undefined field_0x247;
        undefined field_0x248;
        undefined field_0x249;
        undefined field_0x24a;
        undefined field_0x24b;
        undefined field_0x24c;
        undefined field_0x24d;
        undefined field_0x24e;
        undefined field_0x24f;
    };

    struct ScanData
    {
        uint16_t digimonId;
        uint16_t scanrate;
    };

    struct DigimonContext
    {
        void** functions;
        std::map<int64_t, ScanData*> scanData;
        int32_t bankSize;
        undefined field_0x1c;
        undefined field_0x1d;
        undefined field_0x1e;
        undefined field_0x1f;
        std::vector<PartyEntry*> bank;
        std::vector<PartyEntry*> farm[5];
        std::vector<PartyEntry*> party;
        std::vector<uint64_t> unkVector;
        undefined field_0xe0;
        undefined field_0xe1;
        undefined field_0xe2;
        undefined field_0xe3;
        undefined field_0xe4;
        undefined field_0xe5;
        undefined field_0xe6;
        undefined field_0xe7;
        undefined field_0xe8;
        undefined field_0xe9;
        undefined field_0xea;
        undefined field_0xeb;
        undefined field_0xec;
        undefined field_0xed;
        undefined field_0xee;
        undefined field_0xef;
        undefined field_0xf0;
        undefined field_0xf1;
        undefined field_0xf2;
        undefined field_0xf3;
        undefined field_0xf4;
        undefined field_0xf5;
        undefined field_0xf6;
        undefined field_0xf7;
        undefined field_0xf8;
        undefined field_0xf9;
        undefined field_0xfa;
        undefined field_0xfb;
        undefined field_0xfc;
        undefined field_0xfd;
        undefined field_0xfe;
        undefined field_0xff;
        undefined field_0x100;
        undefined field_0x101;
        undefined field_0x102;
        undefined field_0x103;
        undefined field_0x104;
        undefined field_0x105;
        undefined field_0x106;
        undefined field_0x107;
        undefined field_0x108;
        undefined field_0x109;
        undefined field_0x10a;
        undefined field_0x10b;
        undefined field_0x10c;
        undefined field_0x10d;
        undefined field_0x10e;
        undefined field_0x10f;
        undefined field_0x110;
        undefined field_0x111;
        undefined field_0x112;
        undefined field_0x113;
        undefined field_0x114;
        undefined field_0x115;
        undefined field_0x116;
        undefined field_0x117;
    };

    struct Settings
    {
    private:
        void** functions;

    public:
        float music;
        float effect;
        float voice;
        uint32_t minimap;
        uint32_t battleCutscenes;
        uint32_t showPartner;
        uint32_t voices;
        uint32_t difficulty;
        uint32_t unk1;
        uint32_t customBGM[13];
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint32_t unk6;
        uint32_t unk7;
        uint32_t unk8;
        uint32_t unk9;
        uint32_t unk10;
        uint32_t unk11;
        uint32_t unk12;
        float backupMusic;
        float backupEffect;
        float backupVoice;
        uint32_t backupMinimap;
        uint32_t backupBattleCutscenes;
        uint32_t backupShowPartner;
        uint32_t backupVoices;
        uint32_t backupDifficulty;
        uint32_t backupUnk1;
        uint32_t backupCustomBGM[13];
        uint32_t backupUnk2;
        uint32_t backupUnk3;
        uint32_t backupUnk4;
        uint32_t backupUnk5;
        uint32_t backupUnk6;
        uint32_t backupUnk7;
        uint32_t backupUnk8;
        uint32_t backupUnk9;
        uint32_t backupUnk10;
        uint32_t backupUnk11;
        uint32_t backupUnk12;
    };

    struct StoryMode
    {
        void** methods;
        enum StoryModeEnum mode;
        uint32_t field_0xc;
    };

    struct Inventory
    {
        void** methods;
        std::vector<InventoryItem*> bag;
        std::vector<InventoryItem*> field_0x20;
        std::vector<InventoryItem*> field_0x38;
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
        undefined** methods;
        FlagArray* flags;
        DigimonContext* digimonCS;
        struct GameContext_0x18* unk0x18_CS;
        Inventory* inventoryCS;
        struct Workspace* work;
        struct PlayerStruct* playerCS;
        struct QuestsStruct* questsCS;
        struct DigilineStruct* digilineCS;
        struct Settings* settings;
        struct GameContext_0x50* unk0x50;
        struct GameContext_0x58* unk0x58;
        DigimonContext* digimonHM;
        struct GameContext_0x18* unk0x18_HM;
        Inventory* inventoryHM;
        struct PlayerStruct* playerHM;
        struct QuestsStruct* questsHM;
        struct DigilineStruct* digilineHM;
        struct StoryMode* storyMode;
        struct GameContext_0x98* unk0x98;
        struct GameContext_0xa0* unk0xa0;
        struct GameContext_0xa8* unk0xa8;
        struct DLCFlags* dlcflags;
        struct GameContext_0xB8* unk0xb8;
        uint64_t** unk0xc0;
    };
} // namespace dscs