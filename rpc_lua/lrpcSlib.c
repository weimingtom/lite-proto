#include "lrpcSlib.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"


LUA_API slice* rpc_call_func(lua_State* L, slice* arg)
{
	slice* ret = NULL;
	int len=0, i=0, ret_len = 0, b_top =0, h_top=lua_gettop(L);
	llp_mes* arg_ret = NULL;
	llp_mes* arg_list = NULL;
	char* func = NULL;
	char* error = NULL;
	lua_rpc* lr = lua_getrpc(L);
	llp_mes* rpc_lua_call =lr->llp_call;
	llp_mes* rpc_lua_ret = lr->llp_ret;

	// server code
	llp_message_clr(rpc_lua_ret);
	llp_message_clr(rpc_lua_call);

	// get the lite-proto stream from client, and  create message obj by decoding 
	check_fail(llp_in_message(arg, rpc_lua_call), (
		rpc_error(L, "[rpc error]: the lite-proto stream decoding is error at call rpc function!"),	
		NULL
		));	
	func = llp_Rmes_string(rpc_lua_call, "func_call", 0);		// get rpc call function name
	if(func == NULL)
	{
		rpc_error(L, "[rpc error]: the rpc request to call a nil function!");
		return NULL;
	}
	arg_list = llp_Rmes_message(rpc_lua_call, "arg_lua_data", 0);	// get rpc reauest parameter list
	len = llp_Rmes_size(arg_list, "lua_data");
	
	
	lua_getglobal(L, LUA_RPC);
	lua_getfield(L, -1, RPC_REG_TABLE);
	b_top = lua_gettop(L);
	lua_getfield(L, -1, func);
	for(i=len-1; i>=0; i--)
	{
		arg_ret = llp_Rmes_message(arg_list, "lua_data", i);		// get rpc request Parameter
		rpc_out_value(L, arg_ret);									// the parameters is pushed to the lua stack 
	}
	if(lua_pcall(L, len, LUA_MULTRET, 0))
	{
		error = (char*)lua_tostring(L, -1);
		print("[rpc lua error]: %s\n", error);
		llp_Wmes_string(rpc_lua_ret, "ret_error", error);
	}
	else
	{
		ret_len = lua_gettop(L)-b_top;					// get the number of return value
		// get the value from lua stack , and coding
		check_fail(rpc_in(L, ret_len, llp_Wmes_message(rpc_lua_ret, "ret_lua_data")), NULL);		
	}

	lua_settop(L, h_top);
	ret = llp_out_message(rpc_lua_ret);
	return ret;
}
