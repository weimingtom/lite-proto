#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <windows.h>
#include <stdio.h>
#include "llp.h"
#include "lrpclib.h"
#include "lrpcClib.h"


static int lua_t_time(lua_State* L)
{
	lua_pushnumber(L, (int)GetTickCount());
	return 1;
}
int main()
{
	int ret = 0;
	lua_State* L = lua_open();
	luaopen_base(L);
	luaopen_table(L);
	luaopen_rpc(L);

	lua_register(L, "t_time", lua_t_time);
	if(luaL_loadfile(L, "rpc_lua.lua") != 0)
	{
		printf("[error]: %s", lua_tostring(L, -1));
	}
	lua_resume( L, 0 );
	test_con(L);

	lua_close(L);
	mem_print();
	return 0;
}
