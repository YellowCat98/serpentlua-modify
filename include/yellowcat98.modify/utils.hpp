#pragma once
#include <Geode/Geode.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

struct utils {
	static std::string prefixID(lua_State* L, const std::string& str);
};