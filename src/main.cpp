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
    extern std::unordered_map<std::string, Modify::HookInfo> hookRegistry;
    void populateHookRegistry(); // declared here, defined by codegen
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

		sol::state_view state(L);
		sol::environment env(state, sol::create, state.globals());
		env.set_on(function);

		auto result = hookInfo.createHook(L, id, function);

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