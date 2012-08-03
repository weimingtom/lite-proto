#ifndef  _LLP_LUA_H_
#define  _LLP_LUA_H_

#include "llp.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// open env
int llpL_open(lua_State* L, llp_env* env);
// push a message obj
void lua_pushlm(lua_State* L, llp_mes* lm);
// pop a message obj
llp_mes* lua_tolm(lua_State* L, int idx);

#endif