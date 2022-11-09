#pragma once
#include "dscs/DigisterMap.h"
#include "dscs/GameContext.h"
#include <map>

namespace dscs
{
    extern __declspec(dllexport) std::map<int32_t, SeenData>* getSeenData();

    extern __declspec(dllexport) GameContext* getGameContext();

    extern __declspec(dllexport) DigisterMap* getDigisterMap();
} // namespace dscs