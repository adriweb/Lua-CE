#define LUA_LIB

#include "lua.h"

LUALIB_API int luaopen_graphx(lua_State *L);
LUALIB_API int luaopen_keypadc(lua_State *L);
LUALIB_API int luaopen_ti(lua_State *L);
LUALIB_API int luaopen_sys(lua_State *L);
