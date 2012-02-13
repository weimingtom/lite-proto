#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "llp.h"
#include "lrpc.h"
#include "lrpcClib.h"
#include "lrpclib.h"

LUALIB_API int luaopen_rpc(lua_State *L) {
	int top=0;
	int b_len = lua_gettop(L);
	lua_rpc* lr = lua_rpc_new();
	
	check_null(lr, (print("[rpc error]: new rpc is error!\n"), 0));
	
	lua_setrpc(L, lr);
	// new rpc table
	lua_newtable(L);
	top = lua_gettop(L);
	// set rpc function call
	lua_pushcfunction(L, lua_rpc_call);
	lua_setfield(L, top, "call");
	// set rpc regedit function table
	lua_newtable(L);
	lua_setfield(L, top, "reg_func");

	// set rpc table
	lua_setglobal(L, RPC_TABLE);	// set table is rpc
	lua_settop(L, b_len);
	return 1;
}
