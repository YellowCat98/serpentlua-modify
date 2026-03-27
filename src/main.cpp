#include <string>
#include <Windows.h>
#include <sol/sol.hpp>
#include <Geode/Geode.hpp>
#include <yellowcat98.modify/globals.hpp>
#include <yellowcat98.modify/utils.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

SerpentLuaAPI globals::api;

extern "C" __declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	globals::api = toiletAPI;
}

void print(const std::string& msg, const char* type) {
	globals::api.log(globals::api.metadata, msg.c_str(), type);
}



namespace CodegenData {
	inline std::unordered_map<std::string, CodegenDeps::HookInfo> hookRegistry;
	namespace _MenuLayer {
		namespace onMoreGames {
			inline uintptr_t address = 0x335740;
			inline static sol::function hookFn;
			geode::Result<geode::Hook*> createHook(lua_State* L, sol::function fn) {
				sol::state_view state(L);
				hookFn = fn;

				sol::environment env(state, sol::create, state.globals());
				env["original"] = [](MenuLayer* self, cocos2d::CCObject* sender) {
					return self->onMoreGames(sender);
				};

				sol::set_environment(env, hookFn);

				return geode::Mod::get()->hook(
					reinterpret_cast<void*>(geode::base::get() + address), // i should probably add another map that holds what the functions correspond to
					+[](MenuLayer* self, cocos2d::CCObject* sender) {
						hookFn(self, sender);
					},
					utils::prefixID(L, "MenuLayer::onMoreGames"),
					tulip::hook::TulipConvention::Thiscall
				);
			}
		}

		void populateHookRegistry() {
			hookRegistry["MenuLayer_onMoreGames"] = CodegenDeps::HookInfo(CodegenData::_MenuLayer::onMoreGames::address, CodegenData::_MenuLayer::onMoreGames::createHook);
		}
	}

	// hookRegistry is just a teeny tiny way of retrieving a function through lua
	void populateHookRegistry() {
		CodegenData::_MenuLayer::populateHookRegistry();
	}
}




extern "C" __declspec(dllexport) void entry(lua_State* L) {
	globals::api.log(globals::api.metadata, "Modify initialized!", "info");

	sol::state_view state(L);

	auto table = state.create_table();

	CodegenData::populateHookRegistry();

	table["hook"] = [](sol::this_state ts, std::string cls, std::string fn, sol::function function) {
		lua_State* L = ts;
		auto cls_fn = fmt::format("{}_{}", cls, fn);
        if (!CodegenData::hookRegistry.contains(cls_fn)) {
            globals::api.log(globals::api.metadata, fmt::format("{} was not found in hookRegistry.", cls_fn).c_str(), "warn");
        }
		auto& hookInfo = CodegenData::hookRegistry.at(cls_fn);
		
		if (hookInfo.hooked) {
			globals::api.log(globals::api.metadata, fmt::format("{} was already hooked. (hookInfo.hooked == true)", cls_fn).c_str(), "warn");
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

		auto result = hookInfo.createHook(L, function);

		if (result.isErr()) {
			globals::api.log(globals::api.metadata, fmt::format("Unable to apply hook: {}", *(result.err())).c_str(), "error");
			return;
		}
		hookInfo.hooked = true;
	};

	state["serpentlua_modules"][std::string(globals::api.metadata.id)] = [table]() {
		return table;
	};
}