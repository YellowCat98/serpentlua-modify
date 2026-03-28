#include <yellowcat98.modify/utils.hpp>
#include <yellowcat98.modify/Modify.hpp>
#include <yellowcat98.modify/stuff.hpp>

std::string utils::prefixID(lua_State* L, const std::string& str) {
    return fmt::format("{}/{}", Modify::api.get_script(L).id, str);
}