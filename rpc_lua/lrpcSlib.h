#ifndef _LUA_RPC_S_LIB_H_
#define _LUA_RPC_S_LIB_H_
#include "llp.h"
#include "lua.h"

LUA_API slice* rpc_call_func(lua_State* L, slice* arg);
#endif