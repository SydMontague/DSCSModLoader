#include "dscs/GameInterface.h"
#include "dscs/Savegame.h"

#include "modloader/utils.h"

namespace dscs
{
    std::map<uint32_t, SeenData>* getSeenData() {
        return reinterpret_cast<std::map<int32_t, SeenData>*>(getBaseOffset() + 0xF20850);
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
} // namespace dscs