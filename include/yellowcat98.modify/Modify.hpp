#pragma once
#include <yellowcat98.modify/stuff.hpp>
#include <unordered_map>
#include <sol/sol.hpp>
#include <Geode/Geode.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace Modify {
	struct HookEntry {
		std::shared_ptr<geode::Hook> hook; // probably wont be used idk
		std::string id;
		std::string cls;
		std::string fn;
		sol::function babyDetour;
		bool applied;
	};
	struct ScriptContext {
		std::unordered_map<std::string, HookEntry> hooks;
		lua_State* L;
	};

	struct HookInfo {
		HookInfo() = delete;
		HookInfo(
			uintptr_t address,
			geode::Result<std::shared_ptr<geode::Hook>>(*createHook)(lua_State*, const std::string&, sol::function),
			std::vector<sol::function>& fucksRef
		) : address(address), createHook(createHook), fucks(fucksRef) {}
		uintptr_t address;
		geode::Result<std::shared_ptr<geode::Hook>>(*createHook)(lua_State*, const std::string&, sol::function);
		bool hooked; // this only exists so that createHook wouldnt create multiple hooks and instead only add to the call chain
		std::vector<sol::function>& fucks;
	};

	extern SerpentLuaAPI api;
	extern std::unordered_map<lua_State*, ScriptContext> contexts;
};