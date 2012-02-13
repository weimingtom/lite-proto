#include "lrpcSlib.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"


slice* rpc_call_func(lua_State* L, slice* arg)
{
	slice* ret = NULL;
	int len=0, i=0, ret_len = 0, b_top =0;
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
	llp_in_message(arg, rpc_lua_call);							// 获得client 传送过来的rpc包，进行反序列化生成message obj
	func = llp_Rmes_string(rpc_lua_call, "func_call", 0);		// 获得rpc call 的函数名
	if(func == NULL)
	{
		print(CALL_RPC_ERROR, "client try call a null function!");
		return NULL;
	}
	arg_list = llp_Rmes_message(rpc_lua_call, "arg_lua_data", 0);	// 获得参数列表
	len = llp_Rmes_size(arg_list, "lua_data");
	b_top = lua_gettop(L);
	lua_getglobal(L, func);
	for(i=len-1; i>=0; i--)
	{
		arg_ret = llp_Rmes_message(arg_list, "lua_data", i);		// 获得参数
		rpc_out_value(L, arg_ret);									// 将参数展开到stack上
	}
	
	if(lua_pcall(L, len, LUA_MULTRET, 0))
	{
		error = (char*)lua_tostring(L, -1);
		llp_Wmes_string(rpc_lua_ret, "ret_error", error);
		
	}
	else
	{
		ret_len = lua_gettop(L)-b_top;					// 获得返回值个数
		rpc_in(L, ret_len, llp_Wmes_message(rpc_lua_ret, "ret_lua_data"));			// 从栈上获取返回值，同时将其序列化
		lua_settop(L, b_top);
		ret = llp_out_message(rpc_lua_ret);
		//  get in and send to client
	}
	return ret;
}
