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
		HookInfo(uintptr_t address, std::function<geode::Result<geode::Hook*>(sol::function)> createHook) : address(address), createHook(createHook), hooked(false) {}
		uintptr_t address;
		std::function<geode::Result<geode::Hook*>(sol::function)> createHook;
		bool hooked;
	};
};