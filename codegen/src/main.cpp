#include <fmt/core.h>
#include <broma.hpp>
#include <exception>
#include <string>
#include <fstream>
#include <sstream>

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

std::string generateProperHookFnCall(broma::FunctionBindField* fn) {
    std::ostringstream buffer;

    if (fn->prototype.ret.name == "void") {
        buffer << fmt::format("hookFn(self{});", generateArgsFromFn(fn, true, false));
    } else {
        buffer << fmt::format("sol::object result = hookFn(self{});\n", generateArgsFromFn(fn, true, false));
        buffer << fmt::format("                        return result.as<{}>();", fn->prototype.ret.name);
    }

    return buffer.str();
}

std::string generateCreateHook(broma::Class& cls, broma::FunctionBindField* fn) {
    std::ostringstream buffer;
    return fmt::format(
        fmt::runtime(
            "            geode::Result<geode::Hook*>(sol::function fn) {{\n"
            "                hookFn = fn;\n\n"
            "                return geode::Mod::get()->hook(\n"
            "                    reinterpret_cast<void*>(geode::base::get() + address),\n"
            "                    +[]({}* self{}) {{\n"
            "                        sol::state_view state(globals::rawState);\n\n"
            "                        sol::environment env(state, sol::create, state.globals());\n"
            "                        env[\"original\"] = []({}* self{}) {{\n"
            "                            return self->{}({});\n"
            "                        }};\n\n"
            "                        sol::set_environment(env, hookFn);\n"
            "                        {}\n"
            "                    }},\n"
            "                    \"{}::{}\",\n"
            "                    tulip::hook::TulipConvention::Thiscall\n"
            "                );\n"
            "            }}\n"
        ),
        cls.name, generateArgsFromFn(fn, true, true),
        cls.name, generateArgsFromFn(fn, true, true),
        fn->prototype.name, generateArgsFromFn(fn, false, false),
        generateProperHookFnCall(fn),
        cls.name, fn->prototype.name
    );
}

int main() {
    broma::Root root;
    try {
        root = broma::parse_file("test.bro");
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
        for (broma::Field& field : cls.fields) {
            if (auto func = field.get_as<broma::FunctionBindField>()) {
                if (func->prototype.ret.name == "TodoReturn") continue;
                if (func->binds.win == -0x1 || func->binds.win == -0x2) continue; // skip classes that arent bound
                file << fmt::format("        namespace {} {{\n", func->prototype.name);
                
                file << fmt::format("            inline uintptr_t address = {:#x};\n", func->binds.win);
                file << "            inline sol::function hookFn;\n";

                file << generateCreateHook(cls, func); // generation ends here, this doesnt get called

                file << "        }\n";
            }
        }
        file << "    }\n";
    }
    file << "}";

    return 0;
}