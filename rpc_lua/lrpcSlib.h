#ifndef _LUA_RPC_S_LIB_H_
#define _LUA_RPC_S_LIB_H_
#include "llp.h"
#include "lua.h"

LUALIB_API int luaopen_rpc(lua_State *L, lua_CFunction r_cb, lua_CFunction r_error); 
LUALIB_API slice* rpc_call_func(lua_State* L, slice* arg);
#endif