#include "Savegame.h"

#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct GameSaveData
    {
        undefined** methods;
        bool statusThreadActive;
        undefined field2_0x9;
        undefined field3_0xa;
        undefined field4_0xb;
        undefined field5_0xc;
        undefined field6_0xd;
        undefined field7_0xe;
        undefined field8_0xf;
        uint64_t statusThread;
        int32_t statusResponseCode;
        undefined field11_0x1c;
        undefined field12_0x1d;
        undefined field13_0x1e;
        undefined field14_0x1f;
        int32_t statusSlot;
        undefined field16_0x24;
        undefined field17_0x25;
        undefined field18_0x26;
        undefined field19_0x27;
        char* statusData;
        bool saveThreadActive;
        undefined field22_0x31;
        undefined field23_0x32;
        undefined field24_0x33;
        undefined field25_0x34;
        undefined field26_0x35;
        undefined field27_0x36;
        undefined field28_0x37;
        int64_t saveThread;
        int32_t saveResponseCode;
        undefined field31_0x44;
        undefined field32_0x45;
        undefined field33_0x46;
        undefined field34_0x47;
        int32_t saveSlot;
        undefined field36_0x4c;
        undefined field37_0x4d;
        undefined field38_0x4e;
        undefined field39_0x4f;
        struct Savegame* saveSave;
        uint32_t saveSaveSize;
        undefined field42_0x5c;
        undefined field43_0x5d;
        undefined field44_0x5e;
        undefined field45_0x5f;
        int32_t slotData[194];
        bool loadThreadActive;
        undefined field48_0x369;
        undefined field49_0x36a;
        undefined field50_0x36b;
        undefined field51_0x36c;
        undefined field52_0x36d;
        undefined field53_0x36e;
        undefined field54_0x36f;
        uint64_t loadThread;
        int32_t loadResponseCode;
        undefined field57_0x37c;
        undefined field58_0x37d;
        undefined field59_0x37e;
        undefined field60_0x37f;
        int32_t loadSlot;
        undefined field62_0x384;
        undefined field63_0x385;
        undefined field64_0x386;
        undefined field65_0x387;
        struct Savegame* loadSave;
        uint32_t loadSize;
        undefined field68_0x394;
        undefined field69_0x395;
        undefined field70_0x396;
        undefined field71_0x397;
        bool deleteThreadActive;
        undefined field73_0x399;
        undefined field74_0x39a;
        undefined field75_0x39b;
        undefined field76_0x39c;
        undefined field77_0x39d;
        undefined field78_0x39e;
        undefined field79_0x39f;
        uint64_t deleteThread;
        int32_t deleteResponseCode;
        int32_t deleteSlot;
        bool saveSystemThreadActive;
        undefined field84_0x3b1;
        undefined field85_0x3b2;
        undefined field86_0x3b3;
        undefined field87_0x3b4;
        undefined field88_0x3b5;
        undefined field89_0x3b6;
        undefined field90_0x3b7;
        uint64_t saveSystemThread;
        int32_t saveSystemResponseCode;
        undefined field93_0x3c4;
        undefined field94_0x3c5;
        undefined field95_0x3c6;
        undefined field96_0x3c7;
        char* saveSystemData;
        uint32_t saveSystemSize;
        bool loadSystemThreadActive;
        undefined field100_0x3d5;
        undefined field101_0x3d6;
        undefined field102_0x3d7;
        uint64_t loadSystemThread;
        int32_t loadSystemResponseCode;
        undefined field105_0x3e4;
        undefined field106_0x3e5;
        undefined field107_0x3e6;
        undefined field108_0x3e7;
        char* loadSystemData;
        int32_t loadSystemSize;
        undefined field111_0x3f4;
        undefined field112_0x3f5;
        undefined field113_0x3f6;
        undefined field114_0x3f7;
        std::string unk1;
        std::string unk2;
    };

} // namespace dscs