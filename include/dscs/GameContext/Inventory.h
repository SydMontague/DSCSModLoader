#pragma once
#include <stdint.h>
#include <vector>


namespace dscs {
    struct InventoryItem
    {
        bool isFilled;
        char pad[3];
        int32_t slotIndex;
        uint32_t bag;
        uint32_t id;
        int32_t sortValue;
        uint16_t quantity;
        int16_t unk1;
    };

    struct Inventory
    {
        void** methods;
        std::vector<InventoryItem*> bag;
        std::vector<InventoryItem*> unk1;
        std::vector<InventoryItem*> unk2;
    };
}