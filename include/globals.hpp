#pragma once

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

struct globals {
    static lua_State* rawState;
};