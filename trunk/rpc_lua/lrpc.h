#ifndef		_LRPC_LIB_H_
#define		_LRPC_LIB_H_
#include "llp.h"

typedef struct _lua_rpc{
	llp_env* env;
	llp_mes* llp_call;
	llp_mes* llp_ret;
}lua_rpc;


#define  LUA_RPC	    "rpc"
#define  RPC_CALL       "rpc_lua_call"
#define  RPC_RET	    "rpc_lua_ret"

// rpc table value
#define  RPC_REG_TABLE  "reg_func"
#define  RPC_RCALL		"call"
#define  RPC_LR			"lr" 
#define  RPC_CB			"cb"
#define  RPC_ERROR      "error"

#define  RPC_MES_LPB    "rpc_lua.mes.lpb"

enum {
	e_nil,
	e_string,
	e_number,
	e_bool,	
	e_table
};

void rpc_error(lua_State* L, char* error);
int rpc_in(lua_State* L, int arg_len, llp_mes* arg_lua_data);
int rpc_out(lua_State* L, llp_mes* lm);
void rpc_out_value(lua_State* L, llp_mes* rpc_lua_data);
lua_rpc* lua_getrpc(lua_State* L);
void lua_pcall_cb(lua_State* L, slice* in);
#endif