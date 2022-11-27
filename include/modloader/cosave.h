#pragma once

#include "dscs/Savegame.h"

void readSaveFileHook();
void writeSaveFileHook();
void loadSaveHook(void* empty, dscs::Savegame& save);
void createSaveHook(void* empty, dscs::Savegame& save);