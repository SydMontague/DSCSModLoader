#include "DSCSModLoader.h"

#include "ScriptExtension.h"
#include "dscs/GameInterface.h"
#include "modloader/plugin.h"
#include "modloader/utils.h"
#include "dscs/Savegame.h"

#include <Windows.h>

#include <cstdarg>
#include <filesystem>
#include <format>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

// force Windows 7 WINAPI, so Boost::log works. Seems like a Boost bug
#define BOOST_USE_WINAPI_VERSION 0x0601

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>

using PTR_SteamAPI_Init = bool (*)();
PTR_SteamAPI_Init SteamAPI_Init;

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

void initializeLogging()
{
    boost::log::add_file_log(boost::log::keywords::file_name = "DSCSModLoader.log",
                             boost::log::keywords::format =
                                 (boost::log::expressions::stream << "[" << std::setw(8) << std::setfill(' ')
                                                                  << boost::log::trivial::severity << "] "
                                                                  << boost::log::expressions::smessage),
                             boost::log::keywords::auto_flush = true);
    // TODO load from config
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
}

void TestSave(HSQUIRRELVM vm) {
    std::ofstream output("testsave.bin", std::ios::binary);

    dscs::Savegame save{};
    dscs::StorySave& hmSave = save.saveHM;
    dscs::StorySave& csSave = save.saveCS;
    dscs::GameContext* context = dscs::getGameContext();
    std::map<int32_t, dscs::SeenData*>* seenData = dscs::getSeenData();

    save.version = 0x13;
    std::copy(std::begin(context->flags->flags), std::end(context->flags->flags), std::begin(save.flags));
    uint32_t count = 0;
    for(auto it = seenData->begin(); count < 400 && it != seenData->end(); count++, it++)
        save.seenData[count] = *it->second;
    std::copy(std::begin(context->work->flags), std::end(context->work->flags), std::begin(save.work));
    save.settings = context->settings->data;
    save.stats = context->stats->data;
    save.battleBoxData = context->battleBox->data;

    for(int32_t i = 0; i < 6; i++)
    {
        auto& saveBox = save.battleBoxes[i];
        auto& contextBox = context->battleBox->boxes[i];

        saveBox.field0_0x0 = contextBox.field0_0x0;
        std::copy(std::begin(contextBox.name), std::end(contextBox.name), saveBox.name);

        for(int32_t j = 0; j < 11; j++)
            saveBox.party[j] = contextBox.party[j];
    }
    
    // CS Save
    auto digimonCS = context->digimonCS;
    auto& scanDataCS = digimonCS->scanData;
    auto inventoryCS = context->inventoryCS;
    auto questsCS = context->questsCS;
    auto digilineCS = context->digilineCS;
    auto playerCS = context->playerCS;
    auto context0x18CS = context->unk0x18_CS;

    count = 0;
    for(auto it = scanDataCS.begin(); count < 400 && it != scanDataCS.end(); count++, it++)
        csSave.scanData[count] = *it->second;
    
    count = 0;
    for(auto it = digimonCS->bank.begin(); count < 300 && it != digimonCS->bank.end(); count++, it++)
        csSave.bank[count] = (*it);
    csSave.bankSize = digimonCS->bankSize;

    for(int32_t i = 0; i < 5; i++)
    {
        count = 0;
        for(auto it = digimonCS->farm[i].begin(); count < 10 && it != digimonCS->farm[i].end(); count++, it++)
            csSave.farm[i][count] = (*it);
    }

    count = 0;
    for(auto it = digimonCS->party.begin(); count < 11 && it != digimonCS->party.end(); count++, it++)
        csSave.party[count] = (*it);

    count = 0;
    for(auto it = digimonCS->guestDigimon.begin(); count < 2 && it != digimonCS->guestDigimon.end(); count++, it++)
        csSave.guest[count] = (*it);

    csSave.context_0x18 = context0x18CS->data;

    count = 0;
    for(auto it = inventoryCS->bag.begin(); count < 2000 && it != inventoryCS->bag.end(); count++, it++)
        csSave.inventoryBag[count] = *(*it);

    count = 0;
    for(auto it = inventoryCS->unk1.begin(); count < 100 && it != inventoryCS->unk1.end(); count++, it++)
        csSave.inventoryUnk1[count] = *(*it);

    count = 0;
    for(auto it = inventoryCS->unk2.begin(); count < 100 && it != inventoryCS->unk2.end(); count++, it++)
        csSave.inventoryUnk2[count] = *(*it);

    csSave.player = playerCS->data;
    count = 0;
    for(auto it = playerCS->learnedSkills.begin(); count < 30 && it != playerCS->learnedSkills.end(); count++, it++)
        csSave.hackingSkills[count] = *(*it);
    
    count = 0;
    for(auto it = questsCS->unk1.begin(); count < 350 && it != questsCS->unk1.end(); count++, it++)
        csSave.questUnk1[count] = (*it->second);
    count = 0;
    for(auto it = questsCS->unk2.begin(); count < 350 && it != questsCS->unk2.end(); count++, it++)
        csSave.questUnk2[count] = (*it->second);
    count = 0;
    for(auto it = questsCS->unk3.begin(); count < 350 && it != questsCS->unk3.end(); count++, it++)
        csSave.questUnk3[count] = (*it->second);
    csSave.questUnk4 = questsCS->unk4;

    count = 0;
    for(auto it = digilineCS->unk1.begin(); count < 30 && it != digilineCS->unk1.end(); count++, it++)
        csSave.digiline1[count] = *(*it);
    count = 0;
    for(auto it = digilineCS->unk2.begin(); count < 30 && it != digilineCS->unk2.end(); count++, it++)
        csSave.digiline2[count] = *(*it);
    count = 0;
    for(auto it = digilineCS->unk3.begin(); count < 30 && it != digilineCS->unk3.end(); count++, it++)
        csSave.digiline3[count] = *(*it);
    std::copy(digilineCS->field4_0x50.begin(), digilineCS->field4_0x50.end(), std::begin(csSave.digilineUnk));
    
    count = 0;
    for(auto it = digilineCS->field5_0x68.begin(); count < 90 && it != digilineCS->field5_0x68.end(); count++, it++)
        csSave.digiline4[count] = *(*it);
    
    // HM Save
    auto digimonHM = context->digimonHM;
    auto& scanDataHM = digimonHM->scanData;
    auto inventoryHM = context->inventoryHM;
    auto questsHM = context->questsHM;
    auto digilineHM = context->digilineHM;
    auto playerHM = context->playerHM;
    auto context0x18HM = context->unk0x18_HM;

    count = 0;
    for(auto it = scanDataHM.begin(); count < 400 && it != scanDataHM.end(); count++, it++)
        hmSave.scanData[count] = *it->second;
    
    count = 0;
    for(auto it = digimonHM->bank.begin(); count < 300 && it != digimonHM->bank.end(); count++, it++)
        hmSave.bank[count] = (*it);
    hmSave.bankSize = digimonHM->bankSize;

    for(int32_t i = 0; i < 5; i++)
    {
        count = 0;
        for(auto it = digimonHM->farm[i].begin(); count < 10 && it != digimonHM->farm[i].end(); count++, it++)
            hmSave.farm[i][count] = (*it);
    }

    count = 0;
    for(auto it = digimonHM->party.begin(); count < 11 && it != digimonHM->party.end(); count++, it++)
        hmSave.party[count] = (*it);

    count = 0;
    for(auto it = digimonHM->guestDigimon.begin(); count < 2 && it != digimonHM->guestDigimon.end(); count++, it++)
        hmSave.guest[count] = (*it);

    hmSave.context_0x18 = context0x18HM->data;

    count = 0;
    for(auto it = inventoryHM->bag.begin(); count < 2000 && it != inventoryHM->bag.end(); count++, it++)
        hmSave.inventoryBag[count] = *(*it);

    count = 0;
    for(auto it = inventoryHM->unk1.begin(); count < 100 && it != inventoryHM->unk1.end(); count++, it++)
        hmSave.inventoryUnk1[count] = *(*it);

    count = 0;
    for(auto it = inventoryHM->unk2.begin(); count < 100 && it != inventoryHM->unk2.end(); count++, it++)
        hmSave.inventoryUnk2[count] = *(*it);

    hmSave.player = playerHM->data;
    count = 0;
    for(auto it = playerHM->learnedSkills.begin(); count < 30 && it != playerHM->learnedSkills.end(); count++, it++)
        hmSave.hackingSkills[count] = *(*it);
    
    count = 0;
    for(auto it = questsHM->unk1.begin(); count < 350 && it != questsHM->unk1.end(); count++, it++)
        hmSave.questUnk1[count] = (*it->second);
    count = 0;
    for(auto it = questsHM->unk2.begin(); count < 350 && it != questsHM->unk2.end(); count++, it++)
        hmSave.questUnk2[count] = (*it->second);
    count = 0;
    for(auto it = questsHM->unk3.begin(); count < 350 && it != questsHM->unk3.end(); count++, it++)
        hmSave.questUnk3[count] = (*it->second);
    hmSave.questUnk4 = questsHM->unk4;

    count = 0;
    for(auto it = digilineHM->unk1.begin(); count < 30 && it != digilineHM->unk1.end(); count++, it++)
        hmSave.digiline1[count] = *(*it);
    count = 0;
    for(auto it = digilineHM->unk2.begin(); count < 30 && it != digilineHM->unk2.end(); count++, it++)
        hmSave.digiline2[count] = *(*it);
    count = 0;
    for(auto it = digilineHM->unk3.begin(); count < 30 && it != digilineHM->unk3.end(); count++, it++)
        hmSave.digiline3[count] = *(*it);
    std::copy(digilineHM->field4_0x50.begin(), digilineHM->field4_0x50.end(), std::begin(hmSave.digilineUnk));
    
    count = 0;
    for(auto it = digilineHM->field5_0x68.begin(); count < 90 && it != digilineHM->field5_0x68.end(); count++, it++)
        hmSave.digiline4[count] = *(*it);

    output.write(reinterpret_cast<char*>(&save), sizeof(save));
}

void Test(HSQUIRRELVM vm) {
    std::map<int32_t, dscs::SeenData*>* data = dscs::getSeenData();

    for(auto& seenData : *data)
        std::cout << seenData.second->entryId << " " << seenData.second->seenState << " " << seenData.second->field2_0x6 << std::endl;

    data->at(9)->seenState = 0;
}

void DebugLog(HSQUIRRELVM vm, const SQChar* msg) { sq_getprintfunc(vm)(vm, msg); }

void printfunc(HSQUIRRELVM vm, const SQChar* msg, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, msg);
    std::vsnprintf(buffer, 1024, msg, args);

    BOOST_LOG_TRIVIAL(info) << "[Squirrel] " << buffer;
}

class DSCSModLoaderImpl : public DSCSModLoader
{
private:
    std::unordered_map<std::string, std::vector<SquirrelEntry>> squirrelMap;
    std::vector<BasePlugin*> plugins;

    DSCSModLoaderImpl(){};

    void loadPlugin(std::filesystem::path);
    void applyPatchFile(std::filesystem::path);
    void init();
    void squirrelInit(HSQUIRRELVM);
    void archiveListInit();

public:
    DSCSModLoaderImpl(const DSCSModLoaderImpl&) = delete;
    void operator=(const DSCSModLoaderImpl&)    = delete;

    void addSquirrelFunction(const std::string& table, const std::string& name, void** functionPtr, SQFUNCTION closure);
    void addSquirrelFunction(const std::string& table, const std::string& name, SquirrelFunction data);

    // static functions to be used as function pointers
private:
    static void _squirrelInit(HSQUIRRELVM vm);
    static void _archiveListInit();
    static DSCSModLoaderImpl instance;

public:
    static bool bootstrap();
};

DSCSModLoaderImpl DSCSModLoaderImpl::instance;

void DSCSModLoaderImpl::_squirrelInit(HSQUIRRELVM vm) { instance.squirrelInit(vm); }

void DSCSModLoaderImpl::_archiveListInit() { instance.archiveListInit(); }

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

    BasePlugin* pluginInstance = getPluginFunc(this);

    const PluginInfo info = pluginInstance->getPluginInfo();
    BOOST_LOG_TRIVIAL(info) << std::format("Plugin {} {}.{}.{} loaded.",
                                           info.name,
                                           info.version.major,
                                           info.version.minor,
                                           info.version.revision);

    plugins.push_back(pluginInstance);
    pluginInstance->onEnable();
}

void DSCSModLoaderImpl::init()
{
    initializeLogging();

    BOOST_LOG_TRIVIAL(info) << "initializing DSCSModLoader version 0.0.1...";

    // TODO make optional via config
    AllocConsole();
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    auto* data = new dscs::DigisterData();
    // data->map.insert(std::make_pair("useDataBase", "1"));
    // data->map.insert(std::make_pair("defaultMap", "t3001"));
    // data->map.insert(std::make_pair("selectStoryMode", "1"));
    // data->map.insert(std::make_pair("windowsDisplayPlatform", "ORBIS"));
    // data->map.insert(std::make_pair("isDispSpeed", "1"));*/
    dscs::getDigisterMap()->map.insert(std::make_pair("digister", data));

    redirectJump(&_squirrelInit, 0x1FA7AE);

    // script extensions start
    addSquirrelFunction("Debug", "Log", SQUIRREL_AWAY(DebugLog));

    addSquirrelFunction("Debug", "TestSave", SQUIRREL_AWAY(TestSave));
    addSquirrelFunction("Debug", "Test", SQUIRREL_AWAY(Test));

    addSquirrelFunction("Digimon", "GetScan", SQUIRREL_AWAY(dscs::digimon::GetScan));
    addSquirrelFunction("Digimon", "AddScan", SQUIRREL_AWAY(dscs::digimon::AddScan));
    addSquirrelFunction("Digimon", "SetScan", SQUIRREL_AWAY(dscs::digimon::SetScan));
    // script extensions end

    redirectCall(&_archiveListInit, 0x2CF1BA);
    redirectJump(getBaseOffset() + 0x2CF2F1, 0x2CF1C6);

    BOOST_LOG_TRIVIAL(info) << "Loading patches...";

    // loading and applying patch files
    std::filesystem::directory_iterator patches("patches");
    for (auto patch : patches)
        if (patch.is_regular_file()) applyPatchFile(patch.path());

    BOOST_LOG_TRIVIAL(info) << "Loading plugins...";

    // loading plugins
    std::filesystem::directory_iterator plugins("plugins");
    for (auto plugin : plugins)
        if (plugin.is_regular_file()) loadPlugin(plugin.path());

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