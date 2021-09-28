#include "dscs/GameInterface.h"
#include "modloader/utils.h"

namespace dscs {
	GameContext* getGameContext() {
		using Func = dscs::GameContext* (*)();
		const Func f = (Func)(getBaseOffset() + 0x1A70D0);
		return f();
	}
}