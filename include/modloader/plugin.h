#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <modloader/squirrel.h>

// forward declarations
struct Version;
struct PluginInfo;
class DSCSModLoader;
class BasePlugin;

using EnableFunction = void(*)();
using GetPluginFunction = BasePlugin*(*)(DSCSModLoader*);

typedef struct Version {
	uint32_t major;
	uint32_t minor;
	uint32_t revision;
} PluginVersion, APIVersion;

struct PluginInfo {
	APIVersion apiVersion;
	PluginVersion version;
	std::string name; // TODO replace with DSCSString?
};

class BasePlugin {
public:
	BasePlugin(DSCSModLoader* modLoader) : modLoader(modLoader) {};
	~BasePlugin() = default;

	virtual void onEnable() {};
	virtual void onDisable() {};

	const virtual PluginInfo getPluginInfo() = 0;

	DSCSModLoader* modLoader;
};

class DSCSModLoader {
public:
	// squirrel tables
	virtual void addSquirrelFunction(const std::string& table, const std::string& name, void** functionPtr, SQFUNCTION closure) = 0;
	virtual void addSquirrelFunction(const std::string& table, const std::string& name, SquirrelFunction data) = 0;
};