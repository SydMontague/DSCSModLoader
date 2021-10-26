#pragma once
#include "dscs/GameContext.h"
#include "dscs/DigisterMap.h"

namespace dscs {
	extern __declspec(dllexport) GameContext* getGameContext();

	extern __declspec(dllexport) DigisterMap* getDigisterMap();
}