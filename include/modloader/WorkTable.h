#pragma once

#include <algorithm>
#include <array>
#include <iostream>
#include <string>

class WorkTable
{
private:
    char name[0x20]{ 0 };
    std::array<int32_t, 256> data{ 0 };

public:
    WorkTable(std::string tableName)
    {
        std::copy_n(tableName.begin(), std::min(tableName.length(), sizeof(name) - 1), std::begin(name));
        name[sizeof(name) - 1] = '\0';
    }

    auto getName() { return name; }

    int32_t getInt(uint32_t id)
    {
        if (id > data.size()) return 0;
        return data[id];
    }
    float getFloat(uint32_t id)
    {
        if (id > data.size()) return 0;
        return reinterpret_cast<float&>(data[id]);
    }
    void setInt(uint32_t id, int32_t value)
    {
        if (id > data.size()) return;
        data[id] = value;
    }
    void setFloat(uint32_t id, float value)
    {
        if (id > data.size()) return;
        data[id] = reinterpret_cast<int32_t&>(value);
    }
};
