#pragma once

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

struct globals {
    static lua_State* rawState;
};

// things the codegen depends on
namespace CodegenDeps {
	struct HookInfo {
		HookInfo() = default;
		HookInfo(uintptr_t address, geode::Result<geode::Hook*>(*createHook)(sol::function)) : address(address), createHook(createHook), hooked(false) {}
		uintptr_t address;
		geode::Result<geode::Hook*>(*createHook)(sol::function);
		bool hooked;
	};
};