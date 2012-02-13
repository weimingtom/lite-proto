#ifndef  _LUA_RPC_C_LIB_H_
#define  _LUA_RPC_C_LIB_H_

#include "lua.h"
#include "llp.h"

LUA_API int rpc_call_ret(lua_State* L, slice* sl);
#endif