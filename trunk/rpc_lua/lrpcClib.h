#ifndef  _LUA_RPC_C_LIB_H_
#define  _LUA_RPC_C_LIB_H_

#include "lua.h"
#include "llp.h"

LUALIB_API int rpc_call_ret(lua_State* L, slice* sl);
LUALIB_API int rpc_loadfile(lua_State* L, const char* filename);
#endif