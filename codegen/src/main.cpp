#include <fmt/core.h>
#include <broma.hpp>
#include <exception>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

struct globals {
    inline static std::string fileName = "";
    inline static std::string hookRegistryItems = "";
};

std::string generateArgsFromFn(broma::FunctionBindField* fn, bool startWithComma, bool withTypes) {
    std::ostringstream buffer;

    auto args = fn->prototype.args;

    if (startWithComma) buffer << ", ";

    for (const auto& arg : args) {
        auto type = arg.first.name;

        if (withTypes)
            buffer << type << " " << arg.second << ", ";
        else
            buffer << arg.second << ", ";
    }

    auto str = buffer.str();

    if (str.size() >= 2)
        str.erase(str.size() - 2);

    return str;
}

std::string generateHookSignature(broma::Class& cls, broma::FunctionBindField* fn, bool withTypes, bool selfPrefix) {
    if (fn->prototype.is_static) return generateArgsFromFn(fn, false, withTypes);


    std::string ret = withTypes
        ? fmt::format("{}{}", selfPrefix ? fmt::format("{}* self", cls.name) : "", generateArgsFromFn(fn, selfPrefix, withTypes))
        : fmt::format("{}{}", selfPrefix ? "self" : "", generateArgsFromFn(fn, selfPrefix, withTypes));

    return ret;
}

std::string generateOriginalCall(broma::Class& cls, broma::FunctionBindField* fn) {
    std::string finalCall;

    if (fn->prototype.is_static) {
        finalCall = fmt::format("return {}::{}({});", cls.name, fn->prototype.name, generateHookSignature(cls, fn, false, false));
    } else {
        finalCall = fmt::format("return self->{}({});", fn->prototype.name, generateHookSignature(cls, fn, false, false));
    }

    return finalCall;
}

std::string generateProperHookFnCall(broma::Class& cls, broma::FunctionBindField* fn) {
    std::ostringstream buffer;

    if (fn->prototype.ret.name == "void") {
        buffer << fmt::format("hookFn({});", generateHookSignature(cls, fn, false, true));
    } else {
        buffer << fmt::format("sol::object result = hookFn({});\n", generateHookSignature(cls, fn, false, true));
        buffer << fmt::format("                        return result.as<{}>();", fn->prototype.ret.name);
    }

    return buffer.str();
}


std::string generateCreateHook(broma::Class& cls, broma::FunctionBindField* fn) {
    std::ostringstream buffer;
    return fmt::format(
        fmt::runtime(
            "            geode::Result<geode::Hook*> createHook(sol::function fn) {{\n"
            "                hookFn = fn;\n\n"
            "                return geode::Mod::get()->hook(\n"
            "                    reinterpret_cast<void*>(geode::base::get() + address),\n"
            "                    +[]({}) {{\n"
            "                        sol::state_view __theSuperRawLuaState(globals::rawState);\n\n"
            "                        sol::environment env(__theSuperRawLuaState, sol::create, __theSuperRawLuaState.globals());\n"
            "                        env[\"original\"] = []({}) {{\n"
            "                            {}\n"
            "                        }};\n\n"
            "                        sol::set_environment(env, hookFn);\n"
            "                        {}\n"
            "                    }},\n"
            "                    \"{}::{}\",\n"
            "                    tulip::hook::TulipConvention::Thiscall\n"
            "                );\n"
            "            }}\n"
        ),
        generateHookSignature(cls, fn, true, true),
        generateHookSignature(cls, fn, true, true),
        generateOriginalCall(cls, fn),
        generateProperHookFnCall(cls, fn),
        cls.name, fn->prototype.name
    );
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fmt::println("Usage: {} <Broma file>", argv[0]);
        return 1;
    }
    globals::fileName = std::string(argv[1]);

    broma::Root root;
    try {
        root = broma::parse_file(globals::fileName);
    } catch (std::exception& err) {
        fmt::print("{}", err.what());
        return 1;
    }

    std::ofstream file("lindings.cpp", std::ios::app);

    file <<
        "#include <Geode/Geode.hpp>\n"
        "#include <globals.hpp>\n"
        "#include <sol/sol.hpp>\n\n"
        "namespace CodegenData {\n"
        "    inline std::unordered_map<std::string, CodegenDeps::HookInfo> hookRegistry;\n\n";

    for (broma::Class& cls : root.classes) {
        std::string& name = cls.name;
        file << fmt::format("    namespace _{} {{\n", name); // using _{} so it doesnt conflict with existing gd classes

        std::unordered_map<std::string, int> overloadCount;

        for (broma::Field& field : cls.fields) {
            if (auto func = field.get_as<broma::FunctionBindField>()) {
                if (func->prototype.ret.name == "TodoReturn") continue;
                if (func->binds.win == -0x1 || func->binds.win == -0x2) continue; // skip inline and dont have win bindings
                if (func->prototype.type == broma::FunctionType::Ctor || func->prototype.type == broma::FunctionType::Dtor) continue; // ignore constructors and destructors!
                std::string baseName = func->prototype.name;
                int& count = overloadCount[baseName];
                count++;

                std::string finalName = baseName;
                if (count > 1) {
                    finalName += "__";
                    finalName += std::to_string(count);
                }

                file << fmt::format("        namespace {} {{\n", finalName);
                
                file << fmt::format("            inline uintptr_t address = {:#x};\n", func->binds.win);
                file << "            inline sol::function hookFn;\n";

                file << generateCreateHook(cls, func); // generation ends here, this doesnt get called

                globals::hookRegistryItems += fmt::format("        hookRegistry[\"{}_{}\"] = CodegenDeps::HookInfo(CodegenData::_{}::{}::address, CodegenData::_{}::{}::createHook);\n", cls.name, finalName, cls.name, finalName, cls.name, finalName);

                file << "        }\n";
            }
        }
        file << "    }\n";
    }
    file << "    void populateHookRegistry() {\n"
    << globals::hookRegistryItems
    << "    }\n";

    file << "}";

    return 0;
}