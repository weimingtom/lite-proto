#ifndef  _LLP_LUA_H_
#define  _LLP_LUA_H_

#include "llp.h"
#include "lua.h"

typedef enum _llpt{
	LLPT_INT32,
		LLPT_INT64,
		LLPT_FLOAT32,
		LLPT_FLOAT64,
		LLPT_STRING,
		LLPT_STREAM,
		LLPT_MESSAGE
} e_llpt; 

typedef struct _ltype{
	e_llpt t;

	llp_uint8 is_repeated;
	char* filed_name;
}ltype;



// ------- dump a message obj filed type
// lms:		message obj
// idx:		The current filed index at message obj (idx start at 1) 
// lv_out:	the return value
// return:	the next idx (the end is 0)
llp_uint32 llp_Tnext(llp_mes* lms, llp_uint32 idx, ltype* lt_out);

// open env
int llpL_open(lua_State* L, llp_env* env);
// push a message obj
void lua_pushlm(lua_State* L, llp_mes* lm);
// pop a message obj
llp_mes* lua_tolm(lua_State* L, int idx);

#endif