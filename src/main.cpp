#include <string>
#include <Windows.h>
#include <sol/sol.hpp>
#include <Geode/Geode.hpp>
#include <globals.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

lua_State* globals::rawState = nullptr; // initialize the Member globals::rawState as nullptr.

struct __metadata {
    const char* name;
    const char* developer;
    const char* id;
    const char* version;
    const char* serpentVersion;
};

struct SerpentLuaAPI {
    void (*log)(__metadata, const char*, const char*);
    __metadata metadata;
    HMODULE handle;
};

static SerpentLuaAPI api;

extern "C" __declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	api = toiletAPI;
}

void print(const std::string& msg, const char* type) {
	api.log(api.metadata, msg.c_str(), type);
}



namespace CodegenData {
	inline std::unordered_map<std::string, CodegenDeps::HookInfo> hookRegistry;
	namespace _MenuLayer {
		namespace onMoreGames {
			inline uintptr_t address = 0x335740;
			inline static sol::function hookFn;
			geode::Result<geode::Hook*> createHook(sol::function fn) {
				hookFn = fn;

				return geode::Mod::get()->hook(
					reinterpret_cast<void*>(geode::base::get() + address), // i should probably add another map that holds what the functions correspond to
					+[](MenuLayer* self, cocos2d::CCObject* sender) {
						sol::state_view state(globals::rawState);

						sol::environment env(state, sol::create, state.globals());
						env["original"] = [](MenuLayer* self, cocos2d::CCObject* sender) {
							return self->onMoreGames(sender);
						};

						sol::set_environment(env, hookFn);
						hookFn(self, sender);
					},
					"MenuLayer::onMoreGames",
					tulip::hook::TulipConvention::Thiscall
				);
			}
		}
	}

	// hookRegistry is just a teeny tiny way of retrieving a function through lua
	void populateHookRegistry() {
		hookRegistry["MenuLayer_onMoreGames"] = CodegenDeps::HookInfo(CodegenData::_MenuLayer::onMoreGames::address, CodegenData::_MenuLayer::onMoreGames::createHook);
	}
}

extern "C" __declspec(dllexport) void entry(lua_State* L) {
	api.log(api.metadata, "Modify initialized!", "info");

	globals::rawState = L; // but is it now initialized as nullptr? i Dont fucking KNOW!!!!!!!!

	sol::state_view state(globals::rawState);

	auto table = state.create_table();

	CodegenData::populateHookRegistry();

	table["hook"] = [](sol::this_state ts, std::string cls, std::string fn, sol::function function) {
		
		auto cls_fn = fmt::format("{}_{}", cls, fn);

		auto& hookInfo = CodegenData::hookRegistry[cls_fn];
		
		if (hookInfo.hooked) {
			api.log(api.metadata, fmt::format("{} was already hooked. (hookInfo.hooked == true)", cls_fn).c_str(), "warn");
			return;
		}

		/*
		auto result = geode::Mod::get()->hook(
			reinterpret_cast<void*>(geode::base::get() + hookInfo.address), // i should probably add another map that holds what the functions correspond to
			+[](MenuLayer* self, cocos2d::CCObject* sender) {
				sol::state_view state(globals::rawState);

				auto func = globals::hookDetours[cls_fn];

				sol::environment env(state, sol::create, state.globals());
				env["original"] = [](MenuLayer* self, cocos2d::CCObject* sender) {
					return self->onMoreGames(sender);
				};

				sol::set_environment(env, func);
				func(self, sender);
			},
			"Boi",
			tulip::hook::TulipConvention::Thiscall
		);*/

		auto result = hookInfo.createHook(function);

		if (result.isErr()) {
			api.log(api.metadata, fmt::format("Unable to apply hook: {}", *(result.err())).c_str(), "error");
			return;
		}
		hookInfo.hooked = true;
	};

	state["serpentlua_modules"][std::string(api.metadata.id)] = [table]() {
		return table;
	};
}