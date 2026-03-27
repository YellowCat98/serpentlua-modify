#include <yellowcat98.modify/utils.hpp>
#include <yellowcat98.modify/globals.hpp>

std::string utils::prefixID(lua_State* L, const std::string& str) {
    return fmt::format("{}/{}", globals::api.get_script(L).id, str);
}