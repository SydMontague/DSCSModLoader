#include "dscs/GameInterface.h"

#include "dscs/Savegame.h"
#include "modloader/utils.h"

namespace dscs
{
    std::map<int32_t, SeenData*>* getSeenData()
    {
        using Func   = std::map<int32_t, SeenData*>* (*)();
        const Func f = (Func)(getBaseOffset() + 0x1A4740);
        return f();
    }

    GameContext* getGameContext()
    {
        using Func   = GameContext* (*)();
        const Func f = (Func)(getBaseOffset() + 0x1A70D0);
        return f();
    }

    AppContext* getAppContext() { return reinterpret_cast<AppContext*>(getBaseOffset() + 0xF208A0); }

    ResourceManagerContext* getResourceManagerContext()
    {
        return reinterpret_cast<ResourceManagerContext*>(getBaseOffset() + 0xC09188);
    }

    DigisterMap* getDigisterMap()
    {
        using Func   = DigisterMap* (*)();
        const Func f = (Func)(getBaseOffset() + 0x2ccb20);
        return f();
    }

    SoundStruct* getSoundStruct()
    {
        using Func   = SoundStruct* (*)();
        const Func f = (Func)(getBaseOffset() + 0x2bb6d0);
        return f();
    }

    ScanData* setScanData(DigimonContext* context, int32_t digimonId, uint16_t value)
    {
        using Func   = ScanData* (*)(DigimonContext*, int32_t, uint16_t);
        const Func f = (Func)(getBaseOffset() + 0x19aa60);
        return f(context, digimonId, value);
    }

    float getTimeDelta(double* timer)
    {
        using Func   = float (*)(double*);
        const Func f = (Func)(getBaseOffset() + 0x5030d0);
        return f(timer);
    }

    bool useTargetDelta()
    {
        using Func   = bool (*)();
        const Func f = (Func)(getBaseOffset() + 0x51c8c0);
        return f();
    }

    int32_t getTargetFPS()
    {
        using Func   = int32_t (*)();
        const Func f = (Func)(getBaseOffset() + 0x51c8b0);
        return f();
    }

    GameSaveData* getGameSaveData() { return *reinterpret_cast<GameSaveData**>(getBaseOffset() + 0xF20758); }

    int32_t getSaveDirPath(char* buffer)
    {
        using Func   = int32_t (*)(char*);
        const Func f = (Func)(getBaseOffset() + 0x2b9f00);
        return f(buffer);
    }

    void createSaveDir()
    {
        using Func   = void (*)();
        const Func f = (Func)(getBaseOffset() + 0x2ba570);
        return f();
    }

    void encryptSaveFile(void* buffer, std::size_t size, uint64_t key)
    {
        using Func   = void (*)(void*, std::size_t, uint64_t);
        const Func f = (Func)(getBaseOffset() + 0x2cfcf0);
        return f(buffer, size, key);
    }

    void decryptSaveFile(void* buffer, std::size_t size, uint64_t key)
    {
        using Func   = void (*)(void*, std::size_t, uint64_t);
        const Func f = (Func)(getBaseOffset() + 0x2cff70);
        return f(buffer, size, key);
    }

    void endThread(int32_t returnValue)
    {
        using Func   = void (*)(int32_t);
        const Func f = (Func)(getBaseOffset() + 0x8344c8);
        return f(returnValue);
    }

    extern __declspec(dllexport) char* getResourceDir()
    {
        using Func   = char* (*)(void);
        const Func f = (Func)(getBaseOffset() + 0x4f8f60);
        return f();
    }

    extern __declspec(dllexport) std::string getResourceDir(std::string* str)
    {
        using Func   = std::string (*)(std::string*);
        const Func f = (Func)(getBaseOffset() + 0x5F70F0);
        return f(str);
    }

    FarmSaveEntry& FarmSaveEntry::operator=(const PartyEntry& entry)
    {
        this->isFilled   = entry.isFilled;
        this->field4_0x4 = entry.field4_0x4;
        this->field5_0x8 = entry.field5_0x8;
        this->field6_0xc = entry.field6_0xc;
        this->digimonPtr = *entry.digimonPtr;
        return *this;
    }

    FarmSaveEntry& FarmSaveEntry::operator=(const PartyEntry* entry)
    {
        this->isFilled   = entry->isFilled;
        this->field4_0x4 = entry->field4_0x4;
        this->field5_0x8 = entry->field5_0x8;
        this->field6_0xc = entry->field6_0xc;
        this->digimonPtr = *entry->digimonPtr;
        return *this;
    }

    BankSaveEntry& BankSaveEntry::operator=(const PartyEntry& entry)
    {
        this->isFilled   = entry.isFilled;
        this->field4_0x4 = entry.field4_0x4;
        this->field5_0x8 = entry.field5_0x8;
        this->digimonPtr = *entry.digimonPtr;
        return *this;
    }

    BankSaveEntry& BankSaveEntry::operator=(const PartyEntry* entry)
    {
        this->isFilled   = entry->isFilled;
        this->field4_0x4 = entry->field4_0x4;
        this->field5_0x8 = entry->field5_0x8;
        this->digimonPtr = *entry->digimonPtr;
        return *this;
    }

    PartyEntry& PartyEntry::operator=(const BankSaveEntry& entry)
    {
        this->isFilled    = entry.isFilled;
        this->field4_0x4  = entry.field4_0x4;
        this->field5_0x8  = entry.field5_0x8;
        *this->digimonPtr = entry.digimonPtr;
        return *this;
    }

    PartyEntry& PartyEntry::operator=(const BankSaveEntry* entry)
    {
        this->isFilled    = entry->isFilled;
        this->field4_0x4  = entry->field4_0x4;
        this->field5_0x8  = entry->field5_0x8;
        *this->digimonPtr = entry->digimonPtr;
        return *this;
    }

    PartyEntry& PartyEntry::operator=(const FarmSaveEntry& entry)
    {
        this->isFilled    = entry.isFilled;
        this->field4_0x4  = entry.field4_0x4;
        this->field5_0x8  = entry.field5_0x8;
        this->field6_0xc  = entry.field6_0xc;
        *this->digimonPtr = entry.digimonPtr;
        return *this;
    }

    PartyEntry& PartyEntry::operator=(const FarmSaveEntry* entry)
    {
        this->isFilled    = entry->isFilled;
        this->field4_0x4  = entry->field4_0x4;
        this->field5_0x8  = entry->field5_0x8;
        this->field6_0xc  = entry->field6_0xc;
        *this->digimonPtr = entry->digimonPtr;
        return *this;
    }
} // namespace dscs

namespace mediavision
{
    namespace vfs
    {
        void deleteFile(const char* path)
        {
            using Func   = void (*)(const char*);
            const Func f = (Func)(getBaseOffset() + 0x5f6fa0);
            return f(path);
        }

        void createFile(const char* path)
        {
            using Func   = void (*)(const char*);
            const Func f = (Func)(getBaseOffset() + 0x5f6e70);
            return f(path);
        }

        uint64_t writeFile(const char* path, void* buffer, int32_t size)
        {
            using Func   = uint64_t (*)(const char*, void*, int32_t);
            const Func f = (Func)(getBaseOffset() + 0x5f6d30);
            return f(path, buffer, size);
        }

        bool readFile(const char* path, ReadHandle* buffer)
        {
            using Func   = bool (*)(const char*, ReadHandle*);
            const Func f = (Func)(getBaseOffset() + 0x5f6c10);
            return f(path, buffer);
        }
    } // namespace vfs
} // namespace mediavision