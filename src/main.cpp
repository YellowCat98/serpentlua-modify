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
std::unordered_map<lua_State*, ScriptContext> globals::contexts;

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

			static void detour(MenuLayer* self, cocos2d::CCObject* sender) {
				hookFn(self, sender);
			}

			std::shared_ptr<geode::Hook> createHook(lua_State* L, const std::string& id, sol::function fn) {
				sol::state_view state(L);
				hookFn = fn;

				sol::environment env(state, sol::create, state.globals());
				env["original"] = [](MenuLayer* self, cocos2d::CCObject* sender) {
					return self->onMoreGames(sender);
				};

				sol::set_environment(env, fn);

				void* detourPtr = reinterpret_cast<void*>(&_MenuLayer::onMoreGames::detour);

				auto hook = geode::Hook::create(
					reinterpret_cast<void*>(geode::base::get() + address),
					detourPtr,
					fmt::format("[SERPENTLUA MODIFY] MenuLayer::onMoreGames with ID {} for Script {}", utils::prefixID(L, id), globals::api.get_script(L).id),
					tulip::hook::HandlerMetadata{
						.m_convention = geode::hook::createConvention(tulip::hook::TulipConvention::Thiscall),
						.m_abstract = tulip::hook::AbstractFunction::from(static_cast<void(*)(MenuLayer*, cocos2d::CCObject*)>(nullptr)),
					},
					tulip::hook::HookMetadata{}
				);

				return hook;
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

	auto ctx = ScriptContext();
	ctx.L = L;

	globals::contexts[L] = ctx;

	sol::state_view state(L);

	auto table = state.create_table();

	CodegenData::populateHookRegistry();

	table["createHook"] = [](sol::this_state ts, std::string id, std::string cls, std::string fn, sol::function function) {
		lua_State* L = ts;
		auto cls_fn = fmt::format("{}_{}", cls, fn);
        if (!CodegenData::hookRegistry.contains(cls_fn)) {
            globals::api.log(globals::api.metadata, fmt::format("{} was not found in hookRegistry.", cls_fn).c_str(), "warn");
        }
		auto& hookInfo = CodegenData::hookRegistry.at(cls_fn);
		
		if (globals::contexts[L].hooks.contains(utils::prefixID(L, id))) {
			globals::api.log(globals::api.metadata, fmt::format("Cannot create hook with the same ID.", cls_fn).c_str(), "warn");
			return;
		}

		auto result = hookInfo.createHook(L, id, function);

		if (!result) {
			globals::api.log(globals::api.metadata, "Failed to create hook.", "info");
			return;
		}

		globals::contexts[L].hooks[utils::prefixID(L, id)] = result;
	};

	table["applyHook"] = [](sol::this_state ts, std::string id) {
		lua_State* L = ts;
		if (!globals::contexts[L].hooks.contains(utils::prefixID(L, id))) {
			globals::api.log(globals::api.metadata, "Failed to apply hook. Hook was not found.", "info");
			return;
		}

		auto res = globals::contexts[L].hooks[utils::prefixID(L, id)]->enable();
		if (res.isErr()) {
			globals::api.log(globals::api.metadata, fmt::format("Failed to apply hook: {}", *(res.err())).c_str(), "error");
			return;
		}
	};

	state["serpentlua_modules"][std::string(globals::api.metadata.id)] = [table]() {
		return table;
	};
}