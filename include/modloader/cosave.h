#pragma once

#include "dscs/Savegame.h"


void readSaveFile();

void writeSaveFile();

void loadStory(dscs::StorySave& save, bool isHM);

void loadSave(void* empty, dscs::Savegame& save);

void saveStory(dscs::StorySave& save, bool isHM);

void createSave(void* empty, dscs::Savegame& save);