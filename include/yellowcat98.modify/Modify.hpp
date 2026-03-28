#pragma once
#include <yellowcat98.modify/stuff.hpp>
#include <unordered_map>
#include <sol/sol.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace Modify {
	struct ScriptContext {
		std::unordered_map<std::string, std::shared_ptr<geode::Hook>> hooks;
		lua_State* L;
	};

	struct HookInfo {
		HookInfo() = default;
		HookInfo(uintptr_t address, std::shared_ptr<geode::Hook>(*createHook)(lua_State*, const std::string&, sol::function)) : address(address), createHook(createHook) {}
		uintptr_t address;
		std::shared_ptr<geode::Hook>(*createHook)(lua_State*, const std::string&, sol::function);
	};

	extern SerpentLuaAPI api;
	extern std::unordered_map<lua_State*, ScriptContext> contexts;
};