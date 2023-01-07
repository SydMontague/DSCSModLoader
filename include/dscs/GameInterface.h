#pragma once
#include "dscs/AppContext.h"
#include "dscs/DigisterMap.h"
#include "dscs/GameContext.h"
#include "dscs/GameSaveData.h"
#include "dscs/ResourceManager.h"
#include "dscs/SoundStruct.h"

#include <map>

namespace dscs
{
    extern __declspec(dllexport) std::map<int32_t, SeenData*>* getSeenData();

    extern __declspec(dllexport) GameContext* getGameContext();

    extern __declspec(dllexport) DigisterMap* getDigisterMap();

    extern __declspec(dllexport) ScanData* setScanData(DigimonContext* context, int32_t digimonId, uint16_t value);

    extern __declspec(dllexport) SoundStruct* getSoundStruct();

    extern __declspec(dllexport) AppContext* getAppContext();

    extern __declspec(dllexport) ResourceManagerContext* getResourceManagerContext();

    // save stuff
    extern __declspec(dllexport) GameSaveData* getGameSaveData();

    extern __declspec(dllexport) int32_t getSaveDirPath(char* buffer);

    extern __declspec(dllexport) char* getResourceDir();

    extern __declspec(dllexport) std::string getResourceDir(std::string*);

    extern __declspec(dllexport) void createSaveDir();

    extern __declspec(dllexport) void encryptSaveFile(void* buffer, std::size_t size, uint64_t key);

    extern __declspec(dllexport) void decryptSaveFile(void* buffer, std::size_t size, uint64_t key);

    extern __declspec(dllexport) void endThread(int32_t returnValue);

    // data stuff
    extern __declspec(dllexport) float getTimeDelta(double* timer);

    extern __declspec(dllexport) bool useTargetDelta();

    extern __declspec(dllexport) int32_t getTargetFPS();

} // namespace dscs

namespace mediavision
{
    namespace vfs
    {
        struct ReadHandle
        {
            uint8_t* buffer = NULL;
            uint32_t size   = 0;

            ReadHandle(uint8_t* buffer = NULL, uint32_t size = 0)
                : buffer(buffer)
                , size(size)
            {
            }

            ReadHandle(const ReadHandle& other) = delete;

            ReadHandle(ReadHandle&& other) noexcept
            {
                uint8_t* tmpBuffer = buffer;
                buffer             = other.buffer;
                other.buffer       = tmpBuffer;

                uint32_t tmpSize = size;
                size             = other.size;
                other.size       = tmpSize;
            }

            ~ReadHandle()
            {
                if (buffer != NULL)
                {
                    delete buffer;
                    buffer = NULL;
                }
            }

            ReadHandle& operator=(ReadHandle&& other) noexcept
            {
                uint8_t* tmpBuffer = buffer;
                buffer             = other.buffer;
                other.buffer       = tmpBuffer;

                uint32_t tmpSize = size;
                size             = other.size;
                other.size       = tmpSize;
            }
        };

        extern __declspec(dllexport) void deleteFile(const char* path);

        extern __declspec(dllexport) void createFile(const char* path);

        extern __declspec(dllexport) uint64_t writeFile(const char* path, void* buffer, int32_t size);

        extern __declspec(dllexport) bool readFile(const char* path, ReadHandle* buffer);
    } // namespace vfs
} // namespace mediavision