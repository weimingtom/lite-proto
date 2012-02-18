#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lrpc.h"

static int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data);
int lua_rpc_call(lua_State* L);

void rpc_error(lua_State* L, char* error)
{
	int top = lua_gettop(L);	
	lua_getglobal(L, LUA_RPC);
	lua_getfield(L, -1, RPC_ERROR);
	lua_pushstring(L, error);

	if(lua_type(L, -2)==LUA_TNIL || lua_pcall(L, 1, 0, 0))
		lua_error(L);

	lua_settop(L, top);
}

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
	
	if(lua_type(L, -2)== LUA_TNIL)
		rpc_error(L, "[rpc error]: you not set client rpc callback function!");
	else if (lua_pcall(L, 1, 0, 0))
		rpc_error(L, (char*)lua_tostring(L, -1));

	lua_settop(L, top);
}

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

static int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data)
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
	case LUA_TNIL:
		{
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_nil);
		}
		break;
	default:
		{
			char error[128] = {0};
			sprintf(error, "[rpc error]: rpc does not support type:%s", lua_typename(L, inx));
			rpc_error(L, error);
			// set nil filed
			llp_Wmes_int32(rpc_lua_data, "rpc_data_type", e_nil);
		}
		return LP_FAIL;
	}

	return LP_TRUE;
}

int rpc_in(lua_State* L, int arg_len, llp_mes* arg_lua_data)
{
	int i=0;

	check_null(arg_lua_data, LP_FAIL);
	for(i=1; i<=arg_len; i++)
	{
		check_fail(rpc_value_in(L, lua_gettop(L), llp_Wmes_message(arg_lua_data, "lua_data")), (lua_pop(L, 1),LP_FAIL));
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

void rpc_out_value(lua_State* L, llp_mes* rpc_lua_data)
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
	case  e_nil:
		default:
			lua_pushnil(L);
		break;
	}
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


int rpc_new(lua_rpc* ret)
{
	// get message env
	check_null(ret->env=llp_new_env(), (
		print("[rpc error]: the llp new env is error!\n"),
		LP_FAIL));
	// regedit rpc_lua.mes.lpb
	check_fail(llp_reg_mes(ret->env, RPC_MES_LPB), (
		print("[rpc error]: reg %s is error!\n", RPC_MES_LPB), 
		llp_free_env(ret->env),
		LP_FAIL));
	// new rpc_call message obj
	check_null(ret->llp_call=llp_message_new(ret->env, RPC_CALL), (
		print("[rpc error]: not find message: %s!\n", RPC_CALL),
		llp_free_env(ret->env),
		LP_FAIL));
	// new rpc_ret message obj
	check_null(ret->llp_ret=llp_message_new(ret->env, RPC_RET), (
		print("[rpc error]: not find message: %s!\n", RPC_RET),
		llp_message_free(ret->llp_call), llp_free_env(ret->env),
		LP_FAIL));

	return LP_TRUE;
}

void rpc_free(lua_rpc* lr)
{
	if(lr==NULL)
		return;

	llp_message_free(lr->llp_ret);
	llp_message_free(lr->llp_call);
	llp_free_env(lr->env);
}

int lua_rpc_free(lua_State* L)
{
	if(lua_type(L, -1) == LUA_TUSERDATA)
	{
		rpc_free(lua_touserdata(L, -1));
	}
	
	return 0;
}

LUALIB_API int luaopen_rpc(lua_State *L, lua_CFunction r_cb, lua_CFunction r_error) 
{
	int top=0;
	int b_len = lua_gettop(L);
	lua_rpc lr = {0};
	lua_rpc* user_data = NULL;
	
	check_fail(rpc_new(&lr), 0);
	
	// new rpc table
	lua_newtable(L);
	top = lua_gettop(L);

	// set userdata
	user_data = (lua_rpc*)lua_newuserdata(L, sizeof(lr));
	*user_data = lr;

	// set gc
	lua_newtable(L);
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, lua_rpc_free);
	lua_rawset(L, -3);
	lua_setmetatable(L, -2);
	lua_setfield(L, top, RPC_LR);

	// set cb
	(r_cb)?(lua_pushcfunction(L, r_cb)):(lua_pushnil(L));
	lua_setfield(L, top, RPC_CB);

	// set error
	(r_error)?(lua_pushcfunction(L, r_error)):(lua_pushnil(L));
	lua_setfield(L, top, RPC_ERROR);

	// set rpc function call
	lua_pushcfunction(L, lua_rpc_call);
	lua_setfield(L, top, RPC_RCALL);
	
	// set rpc regedit function table
	lua_pushnil(L);
	lua_setfield(L, top, RPC_REG_TABLE);
	
	// set rpc table
	lua_setglobal(L, LUA_RPC);	// set table is rpc
	lua_settop(L, b_len);
	return 1;
}
