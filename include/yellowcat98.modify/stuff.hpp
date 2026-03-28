#pragma once

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