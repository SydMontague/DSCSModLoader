// DSCSModLoader.cpp: Definiert den Einstiegspunkt für die Anwendung.
//

#include <Windows.h>
#include "DSCSModLoader.h"
#include "modloader/plugin.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <format>
#include <filesystem>

#include <cstdarg>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

using PTR_SteamAPI_Init = bool(*)();
PTR_SteamAPI_Init SteamAPI_Init;

struct SquirrelEntry {
	std::string name;
	void** functionPtr;
	SQFUNCTION closurePtr;
};

class DSCSModLoaderImpl : public DSCSModLoader {
private:
	std::unordered_map<std::string, std::vector<SquirrelEntry>> squirrelMap;
public:
	void loadPlugin(std::filesystem::path);
	void applyPatchFile(std::filesystem::path);
	void init();
	void squirrelInit(HSQUIRRELVM);

	void addSquirrelFunction(std::string table, std::string name, void** functionPtr, SQFUNCTION closure);
	void addSquirrelFunction(std::string table, std::string name, SquirrelFunction data);

};

DSCSModLoaderImpl modLoader;

void _squirrelInit(HSQUIRRELVM vm) {
	modLoader.squirrelInit(vm);
}

void patchBytes(const std::vector<uint8_t>& data, const uint64_t offset) {
	DWORD flag = 0;
	size_t size = data.size();

	VirtualProtect(BASE_ADDRESS + offset, size, PAGE_EXECUTE_READWRITE, &flag);
	std::copy_n(data.data(), size, BASE_ADDRESS + offset);
	VirtualProtect(BASE_ADDRESS + offset, size, flag, &flag);
}

void redirectJump(const void* target, const uint64_t offset) {
	std::vector<uint8_t> injectionData = {
		(uint8_t)0x48, (uint8_t)0xB8, ptr2bytes(target),	// MOV RAX,target
		(uint8_t)0xFF, (uint8_t)0xE0,						// JMP RAX
	};

	patchBytes(injectionData, offset);
}

enum class HexToStringResult {
	SUCCESS,
	WRONG_TOKEN_SIZE,
	INVALID_TOKEN
};

HexToStringResult parseByteArray(const std::string& str, std::vector<uint8_t>& vector) {
	std::stringstream sstream(str);
	std::vector<uint8_t> local;

	while (!sstream.eof()) {
		std::string val;
		sstream >> val;

		if (val.size() != 2)
			return HexToStringResult::WRONG_TOKEN_SIZE;

		std::size_t pos;
		local.push_back(std::stoi(val, &pos, 16));

		if (pos != val.size())
			return HexToStringResult::INVALID_TOKEN;
	}

	vector.insert(vector.end(), local.begin(), local.end());
	return HexToStringResult::SUCCESS;
}

void DSCSModLoaderImpl::addSquirrelFunction(std::string table, std::string name, void** functionPtr, SQFUNCTION closure) {
	SquirrelEntry entry;
	entry.name = name;
	entry.functionPtr = functionPtr;
	entry.closurePtr = closure;
	
	squirrelMap.try_emplace(table);
	squirrelMap[table].push_back(entry);

	BOOST_LOG_TRIVIAL(info) << std::format("Function this.{}.{} added", table, name);
}

void DSCSModLoaderImpl::addSquirrelFunction(std::string table, std::string name, SquirrelFunction data) {
	SquirrelEntry entry;
	entry.name = name;
	entry.functionPtr = data.function;
	entry.closurePtr = data.closure;

	squirrelMap.try_emplace(table);
	squirrelMap[table].push_back(entry);

	BOOST_LOG_TRIVIAL(info) << std::format("Function this.{}.{} added", table, name);
}

void DebugLog(HSQUIRRELVM vm, const SQChar* msg) {
	sq_getprintfunc(vm)(vm, msg);
}

void printfunc(HSQUIRRELVM vm, const SQChar* msg, ...) {
	char buffer[1024];
	va_list args;
	va_start(args, msg);
	std::vsnprintf(buffer, 1024, msg, args);

	BOOST_LOG_TRIVIAL(info) << "[Squirrel] " << buffer;
}

void DSCSModLoaderImpl::squirrelInit(HSQUIRRELVM vm) {
	sq_setprintfunc(vm, printfunc);

	for (auto const& [key, val] : squirrelMap) {

		SQObject functionTable = SQObject();
		SQObject root = SQObject();

		// function table
		sq_resetobject(&functionTable);
		sq_newtable(vm);
		sq_getstackobj(vm, -1, &functionTable);
		sq_addref(vm, &functionTable);
		sq_pop(vm, 1);

		for (auto const& function : val) {
			sq_pushobject(vm, functionTable);
			sq_pushstring(vm, function.name.c_str(), -1);
			void** userptr = (void**)sq_newuserdata(vm, 8);
			*userptr = function.functionPtr;

			sq_newclosure(vm, function.closurePtr, 1);
			sq_newslot(vm, -3, SQFalse);
			sq_pop(vm, 1);

			BOOST_LOG_TRIVIAL(info) << std::format("Function this.{}.{} added", key, function.name);
			BOOST_LOG_TRIVIAL(info) << std::hex << function.functionPtr << " " << std::hex << function.closurePtr;
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

void DSCSModLoaderImpl::applyPatchFile(std::filesystem::path file) {
	std::ifstream input(file);
	std::string filename = file.filename().string();

	int lineId = 0;
	std::string line;
	while (std::getline(input, line)) {
		lineId++;
		if (line[0] == ';') // comment
			continue;

		if (line.size() == 0) // empty line
			continue;

		auto splitPos = line.find('=');
		if (splitPos == std::string::npos) { // invalid format
			BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid patch format, must be <offset>=<values>", filename, lineId);
			continue;
		}

		auto left = line.substr(0, splitPos);
		auto right = line.substr(splitPos + 1);

		std::size_t pos;
		auto offset = std::stoll(left, &pos, 16);

		if (pos != left.size()) { // offset invalid format
			BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid offset, must be a hexadecimal number, e.g. 0x123456", filename, lineId);
			continue;
		}

		if (offset < 0 || offset >= 0x1029000) { // sanity/bounds check
			BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] offset out of bounds", filename, lineId);
			continue;
		}

		std::vector<uint8_t> data;
		switch (parseByteArray(right, data)) {
			case HexToStringResult::INVALID_TOKEN:
				BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid payload token", filename, lineId);
				continue;
			case HexToStringResult::WRONG_TOKEN_SIZE:
				BOOST_LOG_TRIVIAL(error) << std::format("[{}:{}] invalid payload token size, must be two hexadecimal numbers per token", filename, lineId);
				continue;
			case HexToStringResult::SUCCESS:
				break;
		}

		BOOST_LOG_TRIVIAL(debug) << std::format("[{}:{}] applied patch successfully", filename, lineId);
		patchBytes(data, offset);
	}
}

void DSCSModLoaderImpl::loadPlugin(std::filesystem::path path) {
	auto plugin = LoadLibrary(path.string().c_str());

	if (plugin == nullptr) {
		BOOST_LOG_TRIVIAL(error) << std::format("Failed to load plugin {}", path.filename().string());
		return;
	}

	GetPluginFunction getPluginFunc = (GetPluginFunction) GetProcAddress(plugin, "getPlugin");

	if (getPluginFunc == nullptr) {
		FreeLibrary(plugin);
		BOOST_LOG_TRIVIAL(error) << std::format("Failed to find 'getPlugin' function for plugin {}", path.filename().string());
		return;
	}

	BasePlugin* pluginInstance = getPluginFunc(this);

	PluginInfo info = pluginInstance->getPluginInfo();
	BOOST_LOG_TRIVIAL(info) << std::format("Plugin {} {}.{}.{} loaded.", info.name, info.version.major, info.version.minor, info.version.revision);

	plugins.push_back(pluginInstance);
	pluginInstance->onEnable();
}

void DSCSModLoaderImpl::init() {
	BOOST_LOG_TRIVIAL(info) << "initializing DSCSModLoader version 0.0.1...";

	redirectJump(&_squirrelInit, 0x1FA7AE);

	// reenable Debug.Log
	addSquirrelFunction("Debug", "Log", SQUIRREL_AWAY(DebugLog));

	BOOST_LOG_TRIVIAL(info) << "Loading patches...";

	// loading and applying patch files
	std::filesystem::directory_iterator patches("patches");
	for (auto patch : patches)
		if (patch.is_regular_file())
			applyPatchFile(patch.path());

	BOOST_LOG_TRIVIAL(info) << "Loading plugins...";

	// loading plugins
	std::filesystem::directory_iterator plugins("plugins");
	for (auto plugin : plugins)
		if (plugin.is_regular_file())
			loadPlugin(plugin.path());

	BOOST_LOG_TRIVIAL(info) << "DSCSModLoader initialized!";
}

void initializeLogging() {
	boost::log::add_file_log(
		boost::log::keywords::file_name = "DSCSModLoader.log",
		boost::log::keywords::format = (
			boost::log::expressions::stream
			<< "[" << std::setw(8) << std::setfill(' ') << boost::log::trivial::severity << "] "
			<< boost::log::expressions::smessage
			),
		boost::log::keywords::auto_flush = true
	);

	boost::log::core::get()->set_filter
	(
		// TODO load from config
		boost::log::trivial::severity >= boost::log::trivial::info
	);
}

bool bootstrap() {
	bool retVal = SteamAPI_Init();
	initializeLogging();
	
	modLoader.init();
	return retVal;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	// redirect SteamAPI_Init call to custom initialization function
	if (fdwReason == DLL_PROCESS_ATTACH) {
		SteamAPI_Init = *((PTR_SteamAPI_Init*) (BASE_ADDRESS + 0x8C09D0));
		const void* ptr = bootstrap;
		std::vector<uint8_t> data = { ptr2bytes(ptr) };
		patchBytes(data, 0x8C09D0);
	}

	return 1;
}