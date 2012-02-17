#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"
#include "lrpcClib.h"

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
	if(llp_in_message(ret, rpc_lua_ret)==LP_FAIL)
	{
		rpc_error(L, "[rpc error]: lite-proto stream parsing error from the server to return!");
	}
	else if((error=llp_Rmes_string(rpc_lua_ret, "ret_error", 0))!=NULL)
	{
		rpc_error(L, error);
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
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_call = NULL;

	check_null(lr, 0);
	rpc_lua_call = lr->llp_call;

	llp_message_clr(rpc_lua_call);														// clear rpc call  message  obj
	rpc_in(L, lua_gettop(L)-1, llp_Wmes_message(rpc_lua_call, "arg_lua_data"));			// get arglist
	if(lua_type(L, 1)!=LUA_TSTRING)		// check func call is string
	{
		rpc_error(L, "[rpc error]: the first parameter is not rpc function string!");
		return 0;
	}
	llp_Wmes_string(rpc_lua_call, "func_call", (char*)lua_tostring(L, 1));
	lua_pop(L, 1);
	lua_pcall_cb(L, llp_out_message(rpc_lua_call));

	return lua_yield( L, 0 );
} 



LUALIB_API int rpc_call_ret(lua_State* L, slice* sl)
{
	int len =0;
	if(lua_status(L) != LUA_YIELD)
		return 0;

	len = _rpc_call_ret(L, sl);
	if(len < 0)	return 0;
	lua_resume(L, len);	
	return 1;
}


LUALIB_API int rpc_loadfile(lua_State* L, const char* filename)
{
	int ret = luaL_loadfile(L, filename);
	
	return (ret)?(ret):(lua_resume(L, 0), 0);
}