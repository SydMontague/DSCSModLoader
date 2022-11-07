#pragma once
#include <stdint.h>

typedef uint8_t undefined;

namespace dscs
{
    struct SettingsData
    {
        float musicVolume;
        float effectVolume;
        float voiceVolume;
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
    };

    struct Settings
    {
    private:
        void** functions;

    public:
        SettingsData data;
        SettingsData backup;
    };
}