#pragma once
#include <unordered_map>

#include <sol/sol.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}


struct __metadata {
    const char* name;
    const char* developer;
    const char* id;
    const char* version;
    const char* serpentVersion;
	const char** plugins;
	int pluginsSize;
};

struct SerpentLuaAPI {
    void (*log)(__metadata, const char*, const char*);
	__metadata (*get_script)(lua_State*);
    __metadata metadata;
    HMODULE handle;
};

struct globals {
	static SerpentLuaAPI api;
};

// things the codegen depends on
namespace CodegenDeps {
	struct HookInfo {
		HookInfo() = default;
		HookInfo(uintptr_t address, geode::Result<geode::Hook*>(*createHook)(lua_State*, sol::function)) : address(address), createHook(createHook), hooked(false) {}
		uintptr_t address;
		geode::Result<geode::Hook*>(*createHook)(lua_State*, sol::function);
		bool hooked;
	};
};