#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"
#include "lrpcClib.h"


#include "lrpcSlib.h"

int _rpc_call_ret(lua_State* L, slice* ret)
{
	int len = 0;
	char* error = NULL;
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_ret = lr->llp_ret;
	
	check_null(ret, 0);
	llp_message_clr(rpc_lua_ret);
	len = llp_in_message(ret, rpc_lua_ret);
	if(error=llp_Rmes_string(rpc_lua_ret, "ret_error", 0))
	{
		print(CALL_RPC_ERROR, error);
		return 0;
	}
	else
	{
		len = rpc_out(L, llp_Rmes_message(rpc_lua_ret, "ret_lua_data", 0));
		return len;
	}
	
	return 0;
}


int lua_rpc_call(lua_State* L)
{
	slice* in = NULL;
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_call =lr->llp_call;

	if(rpc_lua_call == NULL)
		return 0;
	// client  code
	llp_message_clr(rpc_lua_call);														// clear rpc call  message  obj
	rpc_in(L, lua_gettop(L)-1, llp_Wmes_message(rpc_lua_call, "arg_lua_data"));			// get arglist
	if(lua_type(L, 1)!=LUA_TSTRING)		// check func call is string
	{
		print(CALL_RPC_ERROR, "you call func is not string!");
		return 0;
	}
	llp_Wmes_string(rpc_lua_call, "func_call", (char*)lua_tostring(L, 1));
	lua_pop(L, 1);
	in = llp_out_message(rpc_lua_call);
	if(lr->llp_cb)
		lr->llp_cb(in);

	return lua_yield( L, 0 );
} 



LUA_API int rpc_call_ret(lua_State* L, slice* sl)
{
	int len =0;
	if(lua_status(L) != LUA_YIELD)
		return LP_FAIL;

	len = _rpc_call_ret(L, sl);
	lua_resume(L, len);	
	return LP_TRUE;
}
