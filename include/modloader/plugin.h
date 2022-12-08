#pragma once

#include "modloader/FlagTable.h"
#include "modloader/WorkTable.h"
#include "modloader/squirrel.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

// forward declarations
struct Version;
struct PluginInfo;
class DSCSModLoader;
class BasePlugin;

using EnableFunction    = void (*)();
using GetPluginFunction = BasePlugin* (*)(DSCSModLoader*);

using CoSaveReadCallback  = std::function<void(std::vector<uint8_t>)>;
using CoSaveWriteCallback = std::function<std::vector<uint8_t>()>;

typedef struct Version
{
    uint32_t major;
    uint32_t minor;
    uint32_t revision;
} PluginVersion, APIVersion;

struct PluginInfo
{
    APIVersion apiVersion;
    PluginVersion version;
    std::string name;
};

class BasePlugin
{
public:
    BasePlugin(DSCSModLoader& modLoader)
        : modLoader(modLoader){};
    ~BasePlugin() = default;

    virtual void onEnable(){};
    virtual void onDisable(){};

    const virtual PluginInfo getPluginInfo() = 0;

    DSCSModLoader& modLoader;
};

class DSCSModLoader
{
public:
    // squirrel tables
    virtual void
    addSquirrelFunction(const std::string& table, const std::string& name, void** functionPtr, SQFUNCTION closure) = 0;
    virtual void addSquirrelFunction(const std::string& table, const std::string& name, SquirrelFunction data)     = 0;
    virtual void
    addCoSaveHook(const std::string name, CoSaveReadCallback readCallback, CoSaveWriteCallback writeCallback) = 0;

    virtual FlagTable& getFlagTable(std::string& name) = 0;
    virtual WorkTable& getWorkTable(std::string& name) = 0;

public:
    static DSCSModLoader& getInstance();
};
