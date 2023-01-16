#include "DSCSModLoader.h"

#include <toml++/toml.h>

#include "CoSave.h"
#include "CoSaveExtension.hpp"
#include "ScriptExtension.h"
#include "dscs/GameInterface.h"
#include "dscs/Savegame.h"
#include "modloader/plugin.h"
#include "modloader/utils.h"

#include <Windows.h>
#include <stdio.h>

#include <algorithm>
#include <cstdarg>
#include <filesystem>
#include <format>
#include <fstream>
#include <iterator>
#include <list>
#include <map>
#include <ranges>
#include <vector>

// force Windows 7 WINAPI, so Boost::log works. Seems like a Boost bug
#define BOOST_USE_WINAPI_VERSION 0x0601

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

/* Types */
using PTR_SteamAPI_Init = bool (*)();

struct SquirrelEntry
{
    std::string name;
    void** functionPtr    = nullptr;
    SQFUNCTION closurePtr = nullptr;

    SquirrelEntry(const std::string name, void** functionPtr, const SQFUNCTION closurePtr)
        : name(name)
        , functionPtr(functionPtr)
        , closurePtr(closurePtr)
    {
    }
};

class DSCSModLoaderImpl : public DSCSModLoader
{
private:
    toml::table config;
    CoSaveImpl coSave;
    std::unordered_map<std::string, std::vector<SquirrelEntry>> squirrelMap;
    std::vector<BasePlugin*> plugins;

    std::map<std::string, FlagTable> flagTables;
    std::map<std::string, WorkTable> workTables;

    float speedup    = 1.0f;
    int32_t fpsLimit = 60;

private:
    DSCSModLoaderImpl(){};

    void loadPlugin(std::filesystem::path);
    void applyPatchFile(std::filesystem::path);
    void init();
    void squirrelInit(HSQUIRRELVM);
    void archiveListInit();

    std::vector<uint8_t> writeFlagTables();
    void readFlagTables(std::vector<uint8_t>);
    std::vector<uint8_t> writeWorkTables();
    void readWorkTables(std::vector<uint8_t>);
    void loadConfig();
    void initializeLogging();

public:
    DSCSModLoaderImpl(const DSCSModLoaderImpl&) = delete;
    void operator=(const DSCSModLoaderImpl&)    = delete;

    void addSquirrelFunction(const std::string& table, const std::string& name, void** functionPtr, SQFUNCTION closure);
    void addSquirrelFunction(const std::string& table, const std::string& name, SquirrelFunction data);
    void addCoSaveHook(std::string name, CoSaveReadCallback read, CoSaveWriteCallback write);

    FlagTable& getFlagTable(std::string& name);
    WorkTable& getWorkTable(std::string& name);

    float getSpeedup();
    void setSpeedup(float speedup);

    int32_t getFPSLimit();
    void setFPSLimit(int32_t fpsLimit);
    // static functions to be used as function pointers
private:
    static void _squirrelInit(HSQUIRRELVM vm);
    static void _archiveListInit();
    static void _readSaveFileHook();
    static void _writeSaveFileHook();
    static void _loadSaveHook(void* empty, dscs::Savegame& save);
    static void _createSaveHook(void* empty, dscs::Savegame& save);

public:
    static DSCSModLoaderImpl instance;
    static bool bootstrap();
};

/* Variables */
PTR_SteamAPI_Init SteamAPI_Init;
DSCSModLoaderImpl DSCSModLoaderImpl::instance;

/* Functions/Methods */
bool baseUpdateOverride(dscs::AppContext* context)
{
    using Func = bool (*)(dscs::ResourceManager*);
    Func func  = (Func)(getBaseOffset() + 0x500EC0);

    context->timeDelta = dscs::getTimeDelta(&context->timer);
    if (dscs::useTargetDelta()) context->timeDelta = 1.0 / dscs::getTargetFPS();

    context->timeDelta *= DSCSModLoader::getInstance().getSpeedup();

    *reinterpret_cast<float*>(getBaseOffset() + 0xC3AA58) = context->timeDelta;
    *reinterpret_cast<float*>(getBaseOffset() + 0xC3AA5C) = context->timeDelta / 2;

    func(dscs::getResourceManagerContext()->manager);
    return true;
}

void setVSync(bool enable)
{
    using Func = bool (*)(int32_t);
    Func proc  = (Func)dscs::wglGetProcAddress("wglSwapIntervalEXT");
    proc(enable ? 1 : 0);
}

void setMaxFPSOverride(int32_t fps)
{
    if (fps == 60) fps = DSCSModLoader::getInstance().getFPSLimit();
    *reinterpret_cast<int32_t*>(getBaseOffset() + 0xBF5F4C) = fps;
}

int32_t DSCSModLoaderImpl::getFPSLimit() { return fpsLimit; }

void DSCSModLoaderImpl::setFPSLimit(int32_t fpsLimit)
{
    if (fpsLimit <= 0) return;

    this->fpsLimit = fpsLimit;
    setMaxFPSOverride(fpsLimit);
}

float DSCSModLoaderImpl::getSpeedup() { return speedup; }

void DSCSModLoaderImpl::setSpeedup(float speedup)
{
    if (speedup <= 0.1f && speedup >= 20.0f) return;

    this->speedup = speedup;
}

void DSCSModLoaderImpl::initializeLogging()
{
    constexpr auto defaultLevel = boost::log::trivial::info;
    const auto logLevel         = config["ModLoader"]["logLevel"].value_or((int64_t)defaultLevel);

    boost::log::add_file_log(boost::log::keywords::file_name = "DSCSModLoader.log",
                             boost::log::keywords::format =
                                 (boost::log::expressions::stream << "[" << std::setw(8) << std::setfill(' ')
                                                                  << boost::log::trivial::severity << "] "
                                                                  << boost::log::expressions::smessage),
                             boost::log::keywords::auto_flush = true);

    boost::log::core::get()->set_filter(boost::log::trivial::severity >= logLevel);
}

void printfunc(HSQUIRRELVM vm, const SQChar* msg, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, msg);
    std::vsnprintf(buffer, 1024, msg, args);

    BOOST_LOG_TRIVIAL(info) << "[Squirrel] " << buffer;
}

DSCSModLoader& DSCSModLoader::getInstance() { return DSCSModLoaderImpl::instance; }

std::vector<uint8_t> DSCSModLoaderImpl::writeFlagTables()
{
    std::vector<uint8_t> data;

    for (auto& entry : flagTables)
        std::copy_n(reinterpret_cast<uint8_t*>(&entry.second), sizeof(entry.second), std::back_inserter(data));

    return data;
}

void DSCSModLoaderImpl::readFlagTables(std::vector<uint8_t> data)
{
    std::size_t entryCount = data.size() / sizeof(FlagTable);
    FlagTable* itr         = reinterpret_cast<FlagTable*>(data.data());

    flagTables.clear();

    for (int32_t i = 0; i < entryCount; i++)
        flagTables.insert(std::make_pair(std::string(itr->getName()), *itr));
}

std::vector<uint8_t> DSCSModLoaderImpl::writeWorkTables()
{
    std::vector<uint8_t> data;

    for (auto& entry : workTables)
        std::copy_n(reinterpret_cast<uint8_t*>(&entry.second), sizeof(entry.second), std::back_inserter(data));

    return data;
}

void DSCSModLoaderImpl::readWorkTables(std::vector<uint8_t> data)
{
    std::size_t entryCount = data.size() / sizeof(WorkTable);
    WorkTable* itr         = reinterpret_cast<WorkTable*>(data.data());

    workTables.clear();

    for (int32_t i = 0; i < entryCount; i++)
        workTables.insert(std::make_pair(std::string(itr->getName()), *itr));
}

FlagTable& DSCSModLoaderImpl::getFlagTable(std::string& name)
{
    if (name.length() >= 0x20) name = name.substr(0, 0x1F);
    return flagTables.emplace(name, name).first->second;
}

WorkTable& DSCSModLoaderImpl::getWorkTable(std::string& name)
{
    if (name.length() >= 0x20) name = name.substr(0, 0x1F);
    return workTables.emplace(name, name).first->second;
}

void DSCSModLoaderImpl::_squirrelInit(HSQUIRRELVM vm) { instance.squirrelInit(vm); }
void DSCSModLoaderImpl::_archiveListInit() { instance.archiveListInit(); }
void DSCSModLoaderImpl::_readSaveFileHook() { instance.coSave.readSaveFileHook(); }
void DSCSModLoaderImpl::_writeSaveFileHook() { instance.coSave.writeSaveFileHook(); }
void DSCSModLoaderImpl::_loadSaveHook(void* empty, dscs::Savegame& save) { instance.coSave.loadSaveHook(empty, save); }
void DSCSModLoaderImpl::_createSaveHook(void* empty, dscs::Savegame& save)
{
    instance.coSave.createSaveHook(empty, save);
};

void DSCSModLoaderImpl::addCoSaveHook(std::string name, CoSaveReadCallback read, CoSaveWriteCallback write)
{
    coSave.addCoSaveHook(name, read, write);
}

void DSCSModLoaderImpl::archiveListInit()
{
    BOOST_LOG_TRIVIAL(info) << "initializing archive list...";

    uint64_t* archiveCount = (uint64_t*)(getBaseOffset() + 0xF20770);
    char* archiveList      = (char*)(getBaseOffset() + 0xF219C0);
    char** archiveTable    = (char**)(getBaseOffset() + 0xF229C0);
    bool* archiveIsInit    = (bool*)(getBaseOffset() + 0xF20656);

    const auto addArchive = [=](const char* name)
    {
        strcpy_s(archiveList + (*archiveCount * 0x80), 0x7F, name);
        archiveTable[*archiveCount] = archiveList + (*archiveCount * 0x80);
        (*archiveCount)++;
    };

    using GetControllerTypeFunc             = uint64_t (*)();
    GetControllerTypeFunc getControllerType = (GetControllerTypeFunc)(getBaseOffset() + 0x57BFF0);

    auto controllerType = getControllerType();
    auto packed         = config["Archive"]["packed"].as_array();
    auto folder         = config["Archive"]["folder"].as_array();

    if (packed)
    {
        for (auto&& entry : *packed)
        {
            const auto name = entry.as_string()->get().c_str();
            addArchive(name);
            BOOST_LOG_TRIVIAL(info) << "Packed Archive " << name << " added.";
        }
    }
    if (folder)
    {
        for (auto&& entry : *folder)
        {
            const auto name = entry.as_string()->get().c_str();
            addArchive(name);
            dscs::addArchiveOverride(false, name);
            BOOST_LOG_TRIVIAL(info) << "Folder Archive " << name << " added.";
        }
    }

    addArchive("DSDBmod");
    addArchive("DSDBP");
    if (controllerType == 1) addArchive("DSDBSP");
    addArchive("DSDBS");
    addArchive("DSDBA");
    addArchive("DSDB");

    BOOST_LOG_TRIVIAL(info) << "Archive list initialized.";
}

void DSCSModLoaderImpl::addSquirrelFunction(const std::string& table,
                                            const std::string& name,
                                            void** functionPtr,
                                            SQFUNCTION closure)
{
    squirrelMap[table].push_back({ name, functionPtr, closure });
    BOOST_LOG_TRIVIAL(info) << std::format("Function this.{}.{} registered", table, name);
}

void DSCSModLoaderImpl::addSquirrelFunction(const std::string& table, const std::string& name, SquirrelFunction data)
{
    addSquirrelFunction(table, name, data.function, data.closure);
}

void DSCSModLoaderImpl::squirrelInit(HSQUIRRELVM vm)
{
    sq_setprintfunc(vm, printfunc);

    for (auto const& [key, val] : squirrelMap)
    {
        SQObject functionTable = SQObject();
        SQObject root          = SQObject();

        // function table
        sq_resetobject(&functionTable);
        sq_pushroottable(vm);
        sq_pushstring(vm, key.c_str(), -1);

        if (SQ_FAILED(sq_get(vm, -2)))
        {
            sq_newtable(vm);
            BOOST_LOG_TRIVIAL(debug) << std::format("Creating table table '{}'.", key);
        }
        else
            BOOST_LOG_TRIVIAL(debug) << std::format("Modifying table '{}'.", key);

        sq_getstackobj(vm, -1, &functionTable);
        sq_addref(vm, &functionTable);
        sq_pop(vm, 2);

        // adding functions
        for (auto const& function : val)
        {
            sq_pushobject(vm, functionTable);
            sq_pushstring(vm, function.name.c_str(), -1);
            void** userptr = (void**)sq_newuserdata(vm, 8);
            *userptr       = function.functionPtr;

            sq_newclosure(vm, function.closurePtr, 1);
            sq_newslot(vm, -3, SQFalse);
            sq_pop(vm, 1);

            BOOST_LOG_TRIVIAL(info) << std::format("Function '{}.{}' added", key, function.name);
        }

        // root table
        sq_resetobject(&root);
        sq_pushroottable(vm);
        sq_getstackobj(vm, -1, &root);
        sq_addref(vm, &root);
        sq_pop(vm, 1);

        // root table entry
        sq_pushobject(vm, root);
        sq_pushstring(vm, key.c_str(), -1);
        sq_pushobject(vm, functionTable);
        sq_newslot(vm, -3, SQFalse);
        sq_pop(vm, 1);

        // cleanup
        sq_release(vm, &root);
        sq_release(vm, &functionTable);
    }
}

void DSCSModLoaderImpl::applyPatchFile(const std::filesystem::path file)
{
    std::ifstream input(file);
    const std::string filename = file.filename().string();

    int lineId = 0;
    std::string line;
    while (std::getline(input, line))
    {
        lineId++;
        if (line[0] == ';') // comment
            continue;

        if (line.size() == 0) // empty line
            continue;

        auto splitPos = line.find('=');
        if (splitPos == std::string::npos)
        { // invalid format
            BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid patch format, must be <offset>=<values>",
                                                    filename,
                                                    lineId);
            continue;
        }

        const auto left  = line.substr(0, splitPos);
        const auto right = line.substr(splitPos + 1);

        std::size_t pos;
        const auto offset = std::stoll(left, &pos, 16);

        if (pos != left.size())
        { // offset invalid format
            BOOST_LOG_TRIVIAL(error)
                << std::format("[{}:{}] invalid offset, must be a hexadecimal number, e.g. 0x123456", filename, lineId);
            continue;
        }

        if (offset < 0 || offset >= 0x1029000)
        { // sanity/bounds check
            BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] offset out of bounds", filename, lineId);
            continue;
        }

        std::vector<uint8_t> data;
        switch (parseByteArray(right, data))
        {
            case HexToStringResult::INVALID_TOKEN:
                BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid payload token", filename, lineId);
                continue;
            case HexToStringResult::WRONG_TOKEN_SIZE:
                BOOST_LOG_TRIVIAL(error) << std::format(
                    "[{}:{}] invalid payload token size, must be two hexadecimal numbers per token",
                    filename,
                    lineId);
                continue;
            case HexToStringResult::SUCCESS: break;
        }

        BOOST_LOG_TRIVIAL(debug) << std::format("[{}:{}] applied patch successfully", filename, lineId);
        patchBytes(data, offset);
    }
}

void DSCSModLoaderImpl::loadPlugin(const std::filesystem::path path)
{
    const auto plugin = LoadLibrary(path.string().c_str());

    if (plugin == nullptr)
    {
        BOOST_LOG_TRIVIAL(error) << std::format("Failed to load plugin {}", path.filename().string());
        return;
    }

    const GetPluginFunction getPluginFunc = (GetPluginFunction)GetProcAddress(plugin, "getPlugin");

    if (getPluginFunc == nullptr)
    {
        FreeLibrary(plugin);
        BOOST_LOG_TRIVIAL(error) << std::format("Failed to find 'getPlugin' function for plugin {}",
                                                path.filename().string());
        return;
    }

    BasePlugin* pluginInstance = getPluginFunc(*this);

    const PluginInfo info = pluginInstance->getPluginInfo();
    BOOST_LOG_TRIVIAL(info) << std::format("Plugin {} {}.{}.{} loaded.",
                                           info.name,
                                           info.version.major,
                                           info.version.minor,
                                           info.version.revision);

    plugins.push_back(pluginInstance);
    pluginInstance->onEnable();
}

void DSCSModLoaderImpl::loadConfig()
{
    try
    {
        config = toml::parse_file("config.toml");
    }
    catch (const toml::parse_error& err)
    {
        config = toml::table();
    }
}

void DSCSModLoaderImpl::init()
{
    loadConfig();
    initializeLogging();

    BOOST_LOG_TRIVIAL(info) << "initializing DSCSModLoader version 0.0.1...";

    // debug console start
    if (config["ModLoader"]["enableConsole"].value_or(false))
    {
        AllocConsole();
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
    }
    // debug console end

    // Digister Map settings start
    // auto* data = new dscs::DigisterData();
    // data->map.insert(std::make_pair("useDataBase", "1"));
    // data->map.insert(std::make_pair("defaultMap", "t3001"));
    // data->map.insert(std::make_pair("selectStoryMode", "1"));
    // data->map.insert(std::make_pair("windowsDisplayPlatform", "ORBIS"));
    // data->map.insert(std::make_pair("isDispSpeed", "1"));
    // data->map.insert(std::make_pair("notFieldDelete", "1")); // crashes when opening menu?
    // dscs::getDigisterMap()->map.insert(std::make_pair("digister", data));
    // Digister Map settings end

    // Patches start
    if (config["Patches"]["DisableVSync"].value_or(false)) patchBytes({ 0x00 }, 0x2786eb);
    setFPSLimit(config["Patches"]["FPSLimit"].value_or(60));
    setSpeedup(config["Patches"]["Speedup"].value_or(1.0f));

    patchBytes({ 0xB9 }, 0x2d0233);                   // fix custom keyboard layout crash, stol -> stoul
    patchBytes({ 0x63, 0x7D, 0x6B, 0x00 }, 0x582cf1); // FPS Hack #1, read frame delta
    patchBytes({ 0xD9, 0x06, 0xA1, 0x00 }, 0x22a37f); // FPS Hack #2, read frame delta / 2
    patchBytes({ 0x65, 0x19, 0xA1, 0x00 }, 0x2290f3); // FPS Hack #3, read frame delta / 2
    // Patches end

    redirectJump(&baseUpdateOverride, 0x52b6d0);
    redirectJump(&setMaxFPSOverride, 0x27b150);

    // script extensions start
    redirectJump(&_squirrelInit, 0x1FA7AE);

    addSquirrelFunction("Debug", "Log", SQUIRREL_AWAY(dscs::modloader::DebugLog));

    addSquirrelFunction("Digimon", "GetScan", SQUIRREL_AWAY(dscs::digimon::GetScan));
    addSquirrelFunction("Digimon", "AddScan", SQUIRREL_AWAY(dscs::digimon::AddScan));
    addSquirrelFunction("Digimon", "SetScan", SQUIRREL_AWAY(dscs::digimon::SetScan));

    addSquirrelFunction("ModLoader", "SetFlag", SQUIRREL_AWAY(dscs::modloader::SetFlag));
    addSquirrelFunction("ModLoader", "GetFlag", SQUIRREL_AWAY(dscs::modloader::GetFlag));
    addSquirrelFunction("ModLoader", "ClearFlag", SQUIRREL_AWAY(dscs::modloader::ClearFlag));

    addSquirrelFunction("ModLoader", "StorageSetInt", SQUIRREL_AWAY(dscs::modloader::StorageSetInt));
    addSquirrelFunction("ModLoader", "StorageSetFloat", SQUIRREL_AWAY(dscs::modloader::StorageSetFloat));
    addSquirrelFunction("ModLoader", "StorageGetInt", SQUIRREL_AWAY(dscs::modloader::StorageGetInt));
    addSquirrelFunction("ModLoader", "StorageGetFloat", SQUIRREL_AWAY(dscs::modloader::StorageGetFloat));
    // script extensions end

    // custom archive list start
    redirectCall(&_archiveListInit, 0x2CF1BA);
    redirectJump(getBaseOffset() + 0x2CF2F1, 0x2CF1C6);
    // custom archive list end

    // custom save/load start
    redirectJump(&_createSaveHook, 0x288990);
    redirectJump(&_writeSaveFileHook, 0x2bab20);
    redirectJump(&_loadSaveHook, 0x289bb0);
    redirectJump(&_readSaveFileHook, 0x2bae90);

    addCoSaveHook("scanDataCS", readScanDataCS, writeScanDataCS);
    addCoSaveHook("scanDataHM", readScanDataHM, writeScanDataHM);
    addCoSaveHook("seenData", readSeenData, writeSeenData);

    addCoSaveHook(
        "customFlags",
        [this](auto data) { readFlagTables(data); },
        [this] { return writeFlagTables(); });

    addCoSaveHook(
        "customWork",
        [this](auto data) { readWorkTables(data); },
        [this] { return writeWorkTables(); });
    // custom save/load end

    // loading and applying patch files
    BOOST_LOG_TRIVIAL(info) << "Loading patches...";
    std::filesystem::path patches_path(std::format("{}/{}", dscs::getResourceDir(), "/patches"));
    if (std::filesystem::exists(patches_path))
    {
        std::filesystem::directory_iterator patches(patches_path);
        for (auto patch : patches)
            if (patch.is_regular_file()) applyPatchFile(patch.path());
    }
    else { BOOST_LOG_TRIVIAL(info) << "Patches directory not found, skipping."; }

    // loading plugins
    BOOST_LOG_TRIVIAL(info) << "Loading plugins...";
    std::filesystem::path plugins_path(std::format("{}/{}", dscs::getResourceDir(), "/plugins"));
    if (std::filesystem::exists(plugins_path))
    {
        std::filesystem::directory_iterator plugins(plugins_path);
        for (auto plugin : plugins)
            if (plugin.is_regular_file()) loadPlugin(plugin.path());
    }
    else { BOOST_LOG_TRIVIAL(info) << "Plugins directory not found, skipping."; }

    BOOST_LOG_TRIVIAL(info) << "DSCSModLoader initialized!";
}

bool DSCSModLoaderImpl::bootstrap()
{
    const bool retVal = SteamAPI_Init();
    instance.init();
    return retVal;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    // redirect SteamAPI_Init call to custom initialization function
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        SteamAPI_Init             = *((PTR_SteamAPI_Init*)(getBaseOffset() + 0x8C09D0));
        const void* ptr           = DSCSModLoaderImpl::bootstrap;
        std::vector<uint8_t> data = { ptr2bytes(ptr) };
        patchBytes(data, 0x8C09D0);
    }

    return 1;
}
