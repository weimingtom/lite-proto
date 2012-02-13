#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <windows.h>
#include <stdio.h>
#include "llp.h"
#include "lrpcClib.h"
#include "lrpcSlib.h"


static int lua_t_time(lua_State* L)
{
	lua_pushnumber(L, (int)GetTickCount());
	return 1;
}

slice* temp = NULL;
void rpc_call_back(slice* sl)
{
	temp = sl;
}

int main()
{
	int ret = 0;
	lua_State* L = lua_open();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_rpc(L, rpc_call_back);

	lua_register(L, "t_time", lua_t_time);
	// client call
	if(rpc_loadfile(L, "rpc_lua.lua"))
	{
		printf("[error]: %s", lua_tostring(L, -1));
	}
	
	rpc_call_ret(L, rpc_call_func(L, temp));

	lua_close(L);
	mem_print();
	return 0;
}
