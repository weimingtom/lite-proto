#ifndef		_LRPC_LIB_H_
#define		_LRPC_LIB_H_


typedef struct _llp_env llp_env;
typedef struct _llp_mes llp_mes;

typedef struct _lua_rpc{
	llp_env* env;
	llp_mes* llp_call;
	llp_mes* llp_ret;
}lua_rpc;

#define  RPC_TABLE	   "rpc"
#define  RPC_CALL      "rpc_lua_call"
#define  RPC_RET	   "rpc_lua_ret"

#define  RPC_MES_LPB   "rpc_lua.mes.lpb"

#define CALL_RPC_ERROR  "[rpc error]:%s\n"
enum {
	e_null,
	e_string,
	e_number,
	e_bool,	
	e_table
};

lua_rpc* lua_rpc_new();
void lua_rpc_free(lua_rpc* lr);
int rpc_in(lua_State* L, int arg_len, llp_mes* arg_lua_data);
int rpc_out(lua_State* L, llp_mes* lm);
int rpc_value_in(lua_State* L, int inx, llp_mes* rpc_lua_data);
int rpc_out_value(lua_State* L, llp_mes* rpc_lua_data);
#endif