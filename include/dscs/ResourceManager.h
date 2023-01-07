#include <stdint.h>

#include <deque>
#include <map>
#include <string>
#include <vector>

#define NOMINMAX
#include <Windows.h>

typedef uint8_t undefined;

namespace dscs
{

    struct ResourceHandle1Entry
    {
        std::string* name;
        uint8_t field1_0x8;
        undefined field2_0x9;
        undefined field3_0xa;
        undefined field4_0xb;
        undefined field5_0xc;
        undefined field6_0xd;
        undefined field7_0xe;
        undefined field8_0xf;
        uint64_t field9_0x10;
        uint64_t field10_0x18;
    };

    struct AFS2CacheEntry
    {
        std::string name;
        bool isPacked;
        undefined field2_0x21[7];
    };

    struct ResourceManager
    {
        ResourceHandle1Entry handles1[128];
        std::string handles2[128];
        uint16_t handleCount1;
        uint16_t handleCount2;
        bool useDataBase;
        undefined field5_0x2005;
        undefined field6_0x2006;
        undefined field7_0x2007;
        std::vector<AFS2CacheEntry> afs2;
        std::vector<void*> field9_0x2020;
        char imageFolders[8][64];
        int32_t numImageFolder;
        uint8_t field12_0x223c[128][256];
        int32_t field13_0xa23c;
        std::map<std::string, std::string> field14_0xa240;
        std::map<int, void*> field15_0xa250;
        std::map<int, void*> field16_0xa260;
    };

    struct ResourceManagerContext_sub1
    {
        int32_t id;
        int32_t field1_0x4;
    };

    struct ResourceManagerContext
    {
        ResourceManager* manager;
        ResourceManagerContext_sub1 field1_0x8[8];
        _RTL_CRITICAL_SECTION runLock;
        _RTL_CRITICAL_SECTION lock2;
        _RTL_CRITICAL_SECTION lock3;
        std::deque<void*> deque;
        int32_t field6_0xe8;
        bool field7_0xec;
        bool field8_0xed;
        undefined field9_0xee;
        undefined field10_0xef;
        uint64_t threadId;
        int32_t field12_0xf8;
        undefined field13_0xfc;
        undefined field14_0xfd;
        undefined field15_0xfe;
        undefined field16_0xff;
        undefined field17_0x100;
        undefined field18_0x101;
        undefined field19_0x102;
        undefined field20_0x103;
        undefined field21_0x104;
        undefined field22_0x105;
        undefined field23_0x106;
        undefined field24_0x107;
        int32_t keyArea[64]; // ?
        char field26_0x208[128];
    };

} // namespace dscs