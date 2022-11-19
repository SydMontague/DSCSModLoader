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
        using Func   = dscs::GameContext* (*)();
        const Func f = (Func)(getBaseOffset() + 0x1A70D0);
        return f();
    }

    DigisterMap* getDigisterMap()
    {
        using Func   = dscs::DigisterMap* (*)();
        const Func f = (Func)(getBaseOffset() + 0x2ccb20);
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

    void endThread(int32_t returnValue) {
        using Func   = void (*)(int32_t);
        const Func f = (Func)(getBaseOffset() + 0x8344c8);
        return f(returnValue);
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

        bool readFile(const char* path, void* buffer)
        {
            using Func   = bool (*)(const char*, void*);
            const Func f = (Func)(getBaseOffset() + 0x5f6c10);
            return f(path, buffer);
        }
    } // namespace vfs
} // namespace mediavision