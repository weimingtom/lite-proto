#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"
#include "lrpcClib.h"

lua_rpc* lua_getrpc(lua_State* L)
{
	lua_rpc* ret = NULL;
	lua_getglobal((L), LUA_RPC);
	lua_getfield((L), -1, RPC_LR);
	ret = lua_touserdata((L), -1);
	lua_pop(L, 2);

	return ret;
}

void lua_pcall_cb(lua_State* L, slice* in)
{
	int top = lua_gettop(L);
	lua_getglobal(L, LUA_RPC);
	lua_getfield(L, -1, RPC_CB);
	lua_pushlightuserdata(L, in);

	if(lua_pcall(L, 1, 0, 0))
	{
		lua_error(L);
	}

	lua_settop(L, top);
}

int _rpc_call_ret(lua_State* L, slice* ret)
{
	int len = 0;
	char* error = NULL;
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_ret = NULL;

	check_null(lr, -1);
	check_null(ret, -1);
	rpc_lua_ret = lr->llp_ret;
	llp_message_clr(rpc_lua_ret);
	len = llp_in_message(ret, rpc_lua_ret);
	if(error=llp_Rmes_string(rpc_lua_ret, "ret_error", 0))
	{
		lua_pushstring(L, error);
		lua_error(L);
		return -1;
	}
	else
	{
		len = rpc_out(L, llp_Rmes_message(rpc_lua_ret, "ret_lua_data", 0));
		return len;
	}
	
	return -1;
}


int lua_rpc_call(lua_State* L)
{
	slice* in = NULL;
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_call = NULL;

	check_null(lr, 0);
	rpc_lua_call = lr->llp_call;
	// client  code
	llp_message_clr(rpc_lua_call);														// clear rpc call  message  obj
	rpc_in(L, lua_gettop(L)-1, llp_Wmes_message(rpc_lua_call, "arg_lua_data"));			// get arglist
	if(lua_type(L, 1)!=LUA_TSTRING)		// check func call is string
	{
		lua_pushstring(L, "[rpc error]: you call func is not string!");
		lua_error(L);
		return 0;
	}
	llp_Wmes_string(rpc_lua_call, "func_call", (char*)lua_tostring(L, 1));
	lua_pop(L, 1);
	in = llp_out_message(rpc_lua_call);
	lua_pcall_cb(L, in);

	return lua_yield( L, 0 );
} 



LUA_API int rpc_call_ret(lua_State* L, slice* sl)
{
	int len =0;
	if(lua_status(L) != LUA_YIELD)
		return 0;

	len = _rpc_call_ret(L, sl);
	if(len < 0)	return 0;
	lua_resume(L, len);	
	return 1;
}


LUA_API int rpc_loadfile(lua_State* L, const char* filename)
{
	int ret = luaL_loadfile(L, filename);
	
	return (ret)?(ret):(lua_resume(L, 0), 0);
}