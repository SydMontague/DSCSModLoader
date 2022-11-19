#pragma once
#include "dscs/DigisterMap.h"
#include "dscs/GameContext.h"
#include "dscs/GameSaveData.h"

#include <map>

namespace dscs
{
    extern __declspec(dllexport) std::map<int32_t, SeenData*>* getSeenData();

    extern __declspec(dllexport) GameContext* getGameContext();

    extern __declspec(dllexport) DigisterMap* getDigisterMap();

    // save stuff
    extern __declspec(dllexport) GameSaveData* getGameSaveData();

    extern __declspec(dllexport) int32_t getSaveDirPath(char* buffer);

    extern __declspec(dllexport) void createSaveDir();

    extern __declspec(dllexport) void encryptSaveFile(void* buffer, std::size_t size, uint64_t key);

    extern __declspec(dllexport) void decryptSaveFile(void* buffer, std::size_t size, uint64_t key);

    extern __declspec(dllexport) void endThread(int32_t returnValue);
} // namespace dscs

namespace mediavision
{
    namespace vfs
    {
        extern __declspec(dllexport) void deleteFile(const char* path);

        extern __declspec(dllexport) void createFile(const char* path);

        extern __declspec(dllexport) uint64_t writeFile(const char* path, void* buffer, int32_t size);

        extern __declspec(dllexport) bool readFile(const char* path, void* buffer);
    } // namespace vfs
} // namespace mediavision