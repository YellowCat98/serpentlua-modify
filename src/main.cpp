#include <string>
#include <Windows.h>
#include <sol/sol.hpp>
#include <Geode/Geode.hpp>
#include <yellowcat98.modify/stuff.hpp>
#include <yellowcat98.modify/Modify.hpp>
#include <yellowcat98.modify/utils.hpp>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

SerpentLuaAPI Modify::api;
std::unordered_map<lua_State*, Modify::ScriptContext> Modify::contexts;

extern "C" __declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	Modify::api = toiletAPI;
}

void print(const std::string& msg, const char* type) {
	Modify::api.log(Modify::api.metadata, msg.c_str(), type);
}



namespace CodegenData {
	inline std::unordered_map<std::string, Modify::HookInfo> hookRegistry;
	namespace _MenuLayer {
		namespace onMoreGames {
			inline uintptr_t address = 0x335740;
			inline std::vector<sol::function> fucks;
			inline bool hooked = false;

			static void callChain(size_t index, MenuLayer* self, cocos2d::CCObject* sender) {
				if (index >= fucks.size()) {
					self->onMoreGames(sender);
					return;
				}

				auto& fn = fucks[index];

				sol::state_view state(fn.lua_state());
				sol::environment env = sol::get_environment(fn);

				env["original"] = [index](MenuLayer* self, cocos2d::CCObject* sender) {
					callChain(index+1, self, sender);
				};

				fn(self, sender);
			}

			static void detour(MenuLayer* self, cocos2d::CCObject* sender) {
				callChain(0, self, sender);
			}

			geode::Result<std::shared_ptr<geode::Hook>> createHook(lua_State* L, const std::string& id, sol::function fn) {
				sol::state_view state(L);

				void* detourPtr = reinterpret_cast<void*>(&_MenuLayer::onMoreGames::detour);

				std::shared_ptr<geode::Hook> hook;

				if (!_MenuLayer::onMoreGames::hooked) {
					hook = geode::Hook::create(
						reinterpret_cast<void*>(geode::base::get() + address),
						detourPtr,
						"[SERPENTLUA MODIFY] MenuLayer::onMoreGames",
						tulip::hook::HandlerMetadata{
							.m_convention = geode::hook::createConvention(tulip::hook::TulipConvention::Thiscall),
							.m_abstract = tulip::hook::AbstractFunction::from(static_cast<void(*)(MenuLayer*, cocos2d::CCObject*)>(nullptr)),
						},
						tulip::hook::HookMetadata{}
					);
					auto res = hook->enable();
					hooked = true;
					if (res.isErr()) {
						return geode::Err("Failed to enable hook: {}", *(res.err()));
					}
				}

				return geode::Ok(hook);
			}
		}

		void populateHookRegistry() {
			hookRegistry.emplace("MenuLayer_onMoreGames", Modify::HookInfo(CodegenData::_MenuLayer::onMoreGames::address, CodegenData::_MenuLayer::onMoreGames::createHook, CodegenData::_MenuLayer::onMoreGames::fucks));
		}
	}

	// hookRegistry is just a teeny tiny way of retrieving a function through lua
	void populateHookRegistry() {
		CodegenData::_MenuLayer::populateHookRegistry();
	}
}




extern "C" __declspec(dllexport) void entry(lua_State* L) {
	Modify::api.log(Modify::api.metadata, "Modify initialized!", "info");

	auto ctx = Modify::ScriptContext();
	ctx.L = L;

	Modify::contexts[L] = ctx;

	sol::state_view state(L);

	auto table = state.create_table();

	CodegenData::populateHookRegistry();

	table["createHook"] = [](sol::this_state ts, std::string id, std::string cls, std::string fn, sol::function function) {
		lua_State* L = ts;
		auto cls_fn = fmt::format("{}_{}", cls, fn);
        if (!CodegenData::hookRegistry.contains(cls_fn)) {
            Modify::api.log(Modify::api.metadata, fmt::format("{} was not found in hookRegistry.", cls_fn).c_str(), "warn");
			return;
        }
		auto& hookInfo = CodegenData::hookRegistry.at(cls_fn);
		
		if (Modify::contexts[L].hooks.contains(utils::prefixID(L, id))) {
			Modify::api.log(Modify::api.metadata, "Cannot create hook with the same ID.", "warn");
			return;
		}

		auto result = hookInfo.createHook(L, id, function); // no need to check if result is nullptr, nullptr means its already hooked! (or failed lol)

		if (result.isErr()) {
			Modify::api.log(Modify::api.metadata, (*(result.err())).c_str(), "err");
			return;
		}

		Modify::contexts[L].hooks[utils::prefixID(L, id)] = Modify::HookEntry{ // i can actually do this instead of all that bs no way
			.hook = result.unwrap(),
			.id = utils::prefixID(L, id),
			.cls = cls,
			.fn = fn,
			.babyDetour = function,
			.applied = false
		};
	};

	table["applyHook"] = [](sol::this_state ts, std::string id) {
		lua_State* L = ts;
		if (!Modify::contexts[L].hooks.contains(utils::prefixID(L, id))) {
			Modify::api.log(Modify::api.metadata, "Failed to apply hook. Hook was not found.", "error");
			return;
		}

		auto cls_fn = fmt::format("{}_{}", Modify::contexts[L].hooks[utils::prefixID(L, id)].cls, Modify::contexts[L].hooks[utils::prefixID(L, id)].fn);

		auto& hookInfo = CodegenData::hookRegistry.at(cls_fn);

		if (Modify::contexts[L].hooks[utils::prefixID(L, id)].applied) {
			Modify::api.log(Modify::api.metadata, "Cannot call `applyHook` more than once on the same ID.", "warn");
			return;
		}

		hookInfo.fucks.push_back(Modify::contexts[L].hooks[utils::prefixID(L, id)].babyDetour);
		Modify::contexts[L].hooks[utils::prefixID(L, id)].applied = true;
	};

	state["serpentlua_modules"][std::string(Modify::api.metadata.id)] = [table]() {
		return table;
	};
}