#include <fmt/core.h>
#include <broma.hpp>
#include <exception>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

struct globals {
    inline static std::string fileName = "";
    inline static std::string lindingsFolder = "";
    inline static std::string hookRegistryItems = "";
    inline static std::string cmakeScriptItems = "";
};

std::string handleContainerAnnoyingBullshit(std::pair<broma::Type, std::string> arg) {
    auto name = arg.second;
    auto type = arg.first.name;
    std::vector<std::string> bullshit = {
        "std::vector",
        "gd::vector",
        "std::unordered_map",
        "gd::unordered_map",
        "std::list",
        "gd::list"
    };
    
    for (const auto& cont : bullshit) {
        if (type.find(cont) != std::string::npos && type.back() == '*') {
            return fmt::format("*{}", name);
        }
    }

    return name;
}

std::string generateArgsFromFn(broma::FunctionBindField* fn, bool startWithComma, bool withTypes, bool withNames, bool handleAnnoyingBullshit) {
    std::ostringstream buffer;

    auto args = fn->prototype.args;

    if (startWithComma) buffer << ", ";

    for (const auto& arg : args) {
        if (withTypes) {
            if (withNames)
                buffer << arg.first.name << " " << arg.second;
            else
                buffer << arg.first.name;
        } else {
            if (withNames) {
                if (handleAnnoyingBullshit)
                    buffer << handleContainerAnnoyingBullshit(arg);
                else
                    buffer << arg.second;
            }
        }

        buffer << ", ";
    }

    auto str = buffer.str();

    if (str.size() >= 2)
        str.erase(str.size() - 2);

    return str;
}

std::string generateHookSignature(broma::Class& cls, broma::FunctionBindField* fn, bool startWithComma, bool withTypes, bool selfPrefix, bool withNames, bool handleAnnoyingBullshit) {
    if (fn->prototype.is_static) {
        if (fn->prototype.args.empty()) return "";
        return generateArgsFromFn(fn, startWithComma, withTypes, withNames, handleAnnoyingBullshit);
    }


    std::string ret = withTypes
        ? fmt::format("{}{}", selfPrefix ? (withNames ? fmt::format("{}{}* self", (startWithComma ? ", " : ""), cls.name) : fmt::format("{}*", cls.name)) : "", generateArgsFromFn(fn, selfPrefix, withTypes, withNames, handleAnnoyingBullshit))
        : fmt::format("{}{}", selfPrefix ? (startWithComma ? ", self" : "self") : "", generateArgsFromFn(fn, selfPrefix, withTypes, withNames, handleAnnoyingBullshit));

    return ret;
}

std::string generateOriginalCall(broma::Class& cls, broma::FunctionBindField* fn) {
    std::string finalCall;

    if (fn->prototype.is_static) {
        finalCall = fmt::format("return {}::{}({});", cls.name, fn->prototype.name, generateHookSignature(cls, fn, false, false, false, true, false));
    } else {
        finalCall = fmt::format("return self->{}({});", fn->prototype.name, generateHookSignature(cls, fn, false, false, false, true, false));
    }

    return finalCall;
}

std::string generateProperHookFnCall(broma::Class& cls, broma::FunctionBindField* fn) {
    std::ostringstream buffer;

    if (fn->prototype.ret.name == "void") {
        buffer << fmt::format("hookFn({});", generateHookSignature(cls, fn, false, false, true, true, true));
    } else {
        buffer << fmt::format("sol::object __TheGreatResultOfAllTime = hookFn({});\n", generateHookSignature(cls, fn, false, false, true, true, true));
        buffer << fmt::format("                return __TheGreatResultOfAllTime.as<{}>();", fn->prototype.ret.name);
    }

    return buffer.str();
}

bool hasBullshitSol2DoesntLike(broma::FunctionBindField* fn) {
    for (const auto& arg : fn->prototype.args) {
        std::string type = arg.first.name;

        if (type.find("&") != std::string::npos) {
            if (type.find("const") == std::string::npos) {
                return true;
            }
        } else if (type.find("unordered_map") != std::string::npos) return true;
    }
    return false;
}

std::string generateCallChain(broma::Class& cls, broma::FunctionBindField* fn) {
    return fmt::format(
        fmt::runtime(
            "            static {} callChain(size_t __index{}) {{\n"
            "                if (__index >= __fucks.size()) {{\n"
            "                    {}\n"
            "                }}\n\n"
            "                auto& hookFn = __fucks[__index];\n\n"

            "                sol::state_view __AWESOMESTATELETSGO(hookFn.lua_state());\n"
            "                sol::environment env = sol::environment(__AWESOMESTATELETSGO, sol::create, __AWESOMESTATELETSGO.globals());\n\n"
            "                env[\"original\"] = [__index]({}) {{\n"
            "                    return callChain(__index+1{});\n"
            "                }};\n"
            "                sol::set_environment(env, hookFn);\n"
            "                {}\n"
            "            }}\n"
        ),
        fn->prototype.ret.name,
        generateHookSignature(cls, fn, true, true, true, true, false),
        generateOriginalCall(cls, fn),
        generateHookSignature(cls, fn, false, true, true, true, false),
        generateHookSignature(cls, fn, true, false, true, true, false),
        generateProperHookFnCall(cls, fn)
    );
}

std::string generateDetour(broma::Class& cls, broma::FunctionBindField* fn) {
    return fmt::format(
        "            static {} detour({}) {{\n"
        "                return callChain(0{});\n"
        "            }}\n\n",
        fn->prototype.ret.name,
        generateHookSignature(cls, fn, false, true, true, true, false),
        generateHookSignature(cls, fn, true, false, true, true, false)
    );
}

std::string generateCreateHook(broma::Class& cls, broma::FunctionBindField* fn) {
    std::ostringstream buffer;
    return fmt::format(
        fmt::runtime(
            "            geode::Result<std::shared_ptr<geode::Hook>> createHook(lua_State* L, const std::string& id, sol::function fn) {{\n"
            "                sol::state_view __AWESOMESTATELETSGO(L);\n\n"
            "                void* detourPtr = reinterpret_cast<void*>(&_{}::{}::detour);\n\n"
            "                std::shared_ptr<geode::Hook> __hook;\n\n"
            "                if (!__hooked) {{\n"
            "                    __hook = geode::Hook::create(\n"
            "                        reinterpret_cast<void*>(geode::base::get() + address),\n"
            "                        detourPtr,\n"
            "                        \"[SERPENTLUA MODIFY] {}::{}\",\n"
            "                        tulip::hook::HandlerMetadata{{\n"
            "                            .m_convention = geode::hook::createConvention(tulip::hook::TulipConvention::Thiscall),\n"
            "                            .m_abstract = tulip::hook::AbstractFunction::from(static_cast<{}(*)({})>(nullptr)),\n"
            "                        }},\n"
            "                        tulip::hook::HookMetadata{{}}\n"
            "                    );\n"
            "                    auto res = __hook->enable();\n"
            "                    __hooked = true;\n"
            "                    if (res.isErr()) {{\n"
            "                        return geode::Err(\"Failed to enable hook: {{}}\", *(res.err()));\n"
            "                    }}\n"
            "                }}\n"
            "                return geode::Ok(__hook);\n"
            "            }}\n\n"
        ),
        cls.name, fn->prototype.name,
        cls.name, fn->prototype.name,
        fn->prototype.ret.name,
        generateHookSignature(cls, fn, false, true, true, false, false)
    );
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fmt::println("Usage: {} <Broma file>", argv[0]);
        return 1;
    }
    globals::fileName = std::string(argv[1]);
    globals::lindingsFolder = fmt::format("{}.lindings", globals::fileName);

    broma::Root root;
    try {
        root = broma::parse_file(globals::fileName);
    } catch (std::exception& err) {
        fmt::print("{}", err.what());
        return 1;
    }

    std::filesystem::create_directory(globals::lindingsFolder);

    std::ofstream father(fmt::format("{}/__lindings_father.cpp", globals::lindingsFolder), std::ios::app);
    
    father <<
        "// Forward declares populateHookRegistry for every class and defines populateHookRegistry.\n"
        "#include <Geode/Geode.hpp>\n"
        "#include <yellowcat98.modify/Modify.hpp>\n"
        "#include <yellowcat98.modify/stuff.hpp>\n"
        "#include <yellowcat98.modify/utils.hpp>\n"
        "#include <sol/sol.hpp>\n\n"
        "namespace CodegenData {\n"
        "    std::unordered_map<std::string, Modify::HookInfo> hookRegistry;\n\n";

        globals::cmakeScriptItems += fmt::format("    {}/__lindings_father.cpp\n", globals::lindingsFolder);


    for (broma::Class& cls : root.classes) {
        std::string hookRegistryForClass;
        std::string& name = cls.name;
        father << fmt::format("    namespace _{} {{ void populateHookRegistry(); }}\n", name);
        std::ofstream file(fmt::format("{}/lindings_{}.cpp", globals::lindingsFolder, name), std::ios::app);

        file <<
            "// Definition of class \"" << name << "\".\n"
            "#include <Geode/Geode.hpp>\n"
            "#include <yellowcat98.modify/Modify.hpp>\n"
            "#include <yellowcat98.modify/stuff.hpp>\n"
            "#include <yellowcat98.modify/utils.hpp>\n"
            "#include <sol/sol.hpp>\n\n"
            "namespace CodegenData {\n"
            "    extern std::unordered_map<std::string, Modify::HookInfo> hookRegistry;\n\n";

        file << fmt::format("    namespace _{} {{\n", name); // using _{} so it doesnt conflict with existing gd classes

        std::unordered_map<std::string, int> overloadCount;

        for (broma::Field& field : cls.fields) {
            if (auto func = field.get_as<broma::FunctionBindField>()) {
                if (func->prototype.ret.name == "TodoReturn") continue;
                if (func->binds.win == -0x1 || func->binds.win == -0x2) continue; // skip inline and dont have win bindings
                if (func->prototype.type == broma::FunctionType::Ctor || func->prototype.type == broma::FunctionType::Dtor) continue; // ignore constructors and destructors!
                if (hasBullshitSol2DoesntLike(func)) continue; // fuck your std::vector<int>&
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
                file << "            inline std::vector<sol::function> __fucks;\n";
                file << "            inline bool __hooked = false;\n\n";

                file << generateCallChain(cls, func);

                file << generateDetour(cls, func);

                file << generateCreateHook(cls, func);

                hookRegistryForClass += fmt::format("        hookRegistry.emplace(\"{}_{}\", Modify::HookInfo(CodegenData::_{}::{}::address, CodegenData::_{}::{}::createHook, CodegenData::_{}::{}::__fucks));\n", cls.name, finalName, cls.name, finalName, cls.name, finalName, cls.name, finalName);

                file << "        }\n\n";
            }
        }
        file
            << "        void populateHookRegistry() {\n"
            << hookRegistryForClass
            << "        }\n\n";

        globals::hookRegistryItems += fmt::format("        CodegenData::_{}::populateHookRegistry();\n", cls.name);
        globals::cmakeScriptItems += fmt::format("    {}/lindings_{}.cpp\n", globals::lindingsFolder, cls.name);
        file << "    }\n}";
    }
    father << "    void populateHookRegistry() {\n"
    << globals::hookRegistryItems
    << "    }\n";

    father << "}";

    // now onto generating the #CMakeScript

    std::ofstream file(fmt::format("{}/__lindings_generated.cmake", globals::lindingsFolder), std::ios::app);
    file
        << "set(__GENERATED_LINDINGS_SOURCES\n"
        << globals::cmakeScriptItems
        << ")";
    return 0;
}