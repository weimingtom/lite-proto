#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "llp.h"
#include "lrpc.h"

int lua_rpc_call(lua_State* L);

static int rpc_write_table(lua_State* L, int inx, llp_mes* rpc_table)
{
	if(lua_type(L, inx)!= LUA_TTABLE)
		return LP_FAIL;
	
	lua_pushnil(L);			// the nill as table default key
	while(lua_next(L, inx))
	{
		int top = lua_gettop(L);
		llp_mes* table_value = llp_Wmes_message(rpc_table, "table_value");
		rpc_value_in(L, top, llp_Wmes_message(table_value, "value"));		// write value
		
		top--;
		switch(lua_type(L, top))
		{
		case LUA_TNUMBER:		// if key is number
			llp_Wmes_int32(table_value, "n_key", (llp_int32)lua_tonumber(L, top));
			break;
		case LUA_TSTRING:		// if key is string
			llp_Wmes_string(table_value, "s_key", (char*)lua_tostring(L, top));
			break;
		default: 
			return LP_FAIL;
		}
		lua_pop(L, 1);
	}
	return LP_TRUE;
}

int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data)
{
	int tt = 0;
	switch(tt=lua_type(L, inx))
	{
	case LUA_TSTRING:
		{
			llp_Wmes_int32(rpc_lua_data,  "rpc_data_type", e_string);
			llp_Wmes_string(rpc_lua_data, "rpc_str", (char*)lua_tostring(L, inx));
		}
		break;
	case LUA_TNUMBER:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_number);
			llp_Wmes_int32(rpc_lua_data, "rpc_number", (llp_int32)lua_tonumber(L, inx));
		}
		break;
	case LUA_TBOOLEAN:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_bool);
			llp_Wmes_int32(rpc_lua_data, "rpc_bool", lua_toboolean(L, inx));
		}
		break;
	case LUA_TTABLE:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_table);
			rpc_write_table(L, inx, llp_Wmes_message(rpc_lua_data, "rpc_table"));
		}
		break;
	default:
		{
			int a=0;
			a++;
		}
		break;
	}

	return LP_TRUE;
}

int rpc_in(lua_State* L, int arg_len, llp_mes* arg_lua_data)
{
	int i=0;

	for(i=1; i<=arg_len; i++)
	{
		rpc_value_in(L, lua_gettop(L), llp_Wmes_message(arg_lua_data, "lua_data"));
		lua_pop(L, 1);
	}

	return LP_TRUE;
} 

static int rpc_out_table(lua_State* L, llp_mes* rpc_table)
{
	int i=0;
	int len = llp_Rmes_size(rpc_table, "table_value");
	int top = -1;

	lua_createtable(L, len, len);
	top = lua_gettop(L);
	for(i=0; i<len; i++)
	{
		llp_mes* table_value = llp_Rmes_message(rpc_table, "table_value", i);
		int n_key = llp_Rmes_int32(table_value, "n_key", 0);
		char* s_key = llp_Rmes_string(table_value, "s_key", 0);

		if(s_key)
		{
			rpc_out_value(L, llp_Rmes_message(table_value, "value", 0));		// set value
			lua_setfield(L, top, s_key);										// set key
		}
		else
		{
			lua_pushnumber(L, n_key);
			rpc_out_value(L, llp_Rmes_message(table_value, "value", 0));		// set value
			lua_settable(L, top);
		}
	}
	return 1;
}

int rpc_out_value(lua_State* L, llp_mes* rpc_lua_data)
{
	switch(llp_Rmes_int32(rpc_lua_data, "rpc_data_type", 0))
	{
	case e_string:
			lua_pushstring(L, llp_Rmes_string(rpc_lua_data, "rpc_str", 0));
		break;
	case e_bool:
			lua_pushboolean(L, llp_Rmes_int32(rpc_lua_data, "rpc_bool", 0));
		break;
	case e_number:
			lua_pushnumber(L, llp_Rmes_int32(rpc_lua_data, "rpc_number", 0));
		break;
	case e_table:
			rpc_out_table(L, llp_Rmes_message(rpc_lua_data, "rpc_table", 0));
		break;
	default:
		return 0;
	}
	return 1;
}

int rpc_out(lua_State* L, llp_mes* lm)
{
	int i=0, len=0;
	len = llp_Rmes_size(lm, "lua_data");
	for(i=len-1; i>=0; i--)
	{
		llp_mes* rpc_ret = llp_Rmes_message(lm, "lua_data", i);
		rpc_out_value(L, rpc_ret);
	}

	return len;
}


lua_rpc* lua_rpc_new()
{
	lua_rpc* ret = (lua_rpc*)malloc(sizeof(*ret));
	check_null(ret, NULL);
	ret->llp_cb = NULL;

	// get message env
	check_null(ret->env=llp_new_env(), (
		print("the llp new env is error!\n"), 
		free(ret),
		NULL));
	// regedit rpc_lua.mes.lpb
	check_fail(llp_reg_mes(ret->env, RPC_MES_LPB), (
		print("reg %s is error!", RPC_MES_LPB), 
		llp_free_env(ret->env), free(ret),
		NULL));
	// new rpc_call message obj
	check_null(ret->llp_call=llp_message_new(ret->env, RPC_CALL), (
		print("not find message: %s \n", RPC_CALL),
		llp_free_env(ret->env), free(ret),
		NULL));
	// new rpc_ret message obj
	check_null(ret->llp_ret=llp_message_new(ret->env, RPC_RET), (
		print("not find message: %s\n", RPC_RET),
		llp_message_free(ret->llp_call), llp_free_env(ret->env), free(ret),
		NULL));

	return ret;
}


void lua_rpc_free(lua_rpc* lr)
{
	if(lr==NULL)
		return;

	llp_message_free(lr->llp_ret);
	llp_message_free(lr->llp_call);
	llp_free_env(lr->env);
	free(lr);
}


LUALIB_API int luaopen_rpc(lua_State *L, rpc_cb r_cb) {
	int top=0;
	int b_len = lua_gettop(L);
	lua_rpc* lr = lua_rpc_new();
	
	check_null(lr, (print(CALL_RPC_ERROR, "new rpc is error!"), 0));
	lr->llp_cb = r_cb;
	lua_setrpc(L, lr);
	// new rpc table
	lua_newtable(L);
	top = lua_gettop(L);
	// set rpc function call
	lua_pushcfunction(L, lua_rpc_call);
	lua_setfield(L, top, "call");
	// set rpc regedit function table
	lua_pushnil(L);
	lua_setfield(L, top, RPC_REG_TABLE);
	
	// set rpc table
	lua_setglobal(L, LUA_RPC);	// set table is rpc
	lua_settop(L, b_len);
	return 1;
}
