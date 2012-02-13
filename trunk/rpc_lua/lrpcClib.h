#ifndef  _LUA_RPC_C_LIB_H_
#define  _LUA_RPC_C_LIB_H_

#include "lua.h"
#include "llp.h"

int lua_rpc_call(lua_State* L);
void test_con(lua_State* L);
#endif