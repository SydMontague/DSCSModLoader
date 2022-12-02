#pragma once

#include "dscs/Savegame.h"
#include "modloader/plugin.h"

class CoSaveImpl
{
private:
    std::map<std::string, std::vector<uint8_t>> rawContainer;
    std::map<std::string, std::pair<CoSaveReadCallback, CoSaveWriteCallback>> coSaveCallbacks;

public:
    void readSaveFileHook();
    void writeSaveFileHook();
    void loadSaveHook(void* empty, dscs::Savegame& save);
    void createSaveHook(void* empty, dscs::Savegame& save);
    void addCoSaveHook(std::string name, CoSaveReadCallback read, CoSaveWriteCallback write);

private:
    bool writeCoSave();
    void createCoSave();
    bool readCoSave();
    void writeSaveFile();
    void readSaveFile();
    void createSave(void* empty, dscs::Savegame& save);
    void createStorySave(dscs::StorySave& save, bool isHM);
    void loadSave(void* empty, dscs::Savegame& save);
    void loadStorySave(dscs::StorySave& save, bool isHM);
};
