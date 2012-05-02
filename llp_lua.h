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

typedef struct _lvalue {
	e_llpt t;
	union {
		llp_int32	i32;
		llp_int64	i64;
		llp_float32 f32;
		llp_float64 f64;
		char*		str;
		slice*		stream;
		struct {
			llp_mes*	lm;
			char*	mes_name;
		} mes;
	}v;
	
	llp_uint8 is_repeated;
	char* filed_name;
}lvalue;



// ------- dump a message obj
// lms:		message obj
// idx:		The current filed index at message obj (idx start at 1) 
// lv_out:	the return value
// return:	the next idx (the end is 0)
llp_uint32 llp_next(llp_mes* lms, llp_uint32 idx, lvalue* lv_out);

int llpL_open(lua_State* L, llp_env* env);

#endif