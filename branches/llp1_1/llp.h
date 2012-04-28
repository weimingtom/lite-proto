#ifndef _LLP_H_
#define _LLP_H_
#include "lp_conf.h"


typedef struct _llp_env llp_env;
typedef struct _llp_mes llp_mes;

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


// --------env
llp_env* llp_new_env();
void llp_free_env(llp_env* p);

// --------regedit message from .lpb files
int llp_reg_mes(llp_env* env, char* mes_name);
// --------regedit message from slice
int llp_reg_Smes(llp_env* env, slice* buff);


// --------new/delete a message object
llp_mes*  llp_message_new(llp_env* env, char* mes_name);
void llp_message_clr(llp_mes* in_mes);
void  llp_message_free(llp_mes* in_mes);

// --------write a message object
int llp_Wmes_int32(llp_mes* lm, char* filed_str, llp_int32 number);
int llp_Wmes_int64(llp_mes* lm, char* filed_str, llp_int64 number);
int llp_Wmes_float32(llp_mes* lm, char* filed_str, llp_float32 number);
int llp_Wmes_float64(llp_mes* lm, char* filed_str, llp_float64 number);
int llp_Wmes_string(llp_mes* lm, char* filed_str, char* str);
int llp_Wmes_stream(llp_mes* lm, char* filed_str, unsigned char* ptr, unsigned int len);
llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_str);

// ---------read a me   ssage object
llp_int32 llp_Rmes_int32(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_int64 llp_Rmes_int64(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_float32 llp_Rmes_float32(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_float64 llp_Rmes_float64(llp_mes* lm, char* filed_str, unsigned int al_inx);
slice* llp_Rmes_stream(llp_mes* lm, char* filed_str, unsigned int al_inx);
char* llp_Rmes_string(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_uint32 llp_Rmes_size(llp_mes* lm, char* filed_str);

// ------- out/in a message obj
slice* llp_out_message(llp_mes* lms);
int llp_in_message(slice* in, llp_mes* lms);


// ------- dump a message obj
// lms:		message obj
// idx:		The current filed index at message obj (idx start at 1) 
// lv_out:	the return value
// return:	the next idx (the end is 0)
llp_uint32 llp_next(llp_mes* lms, llp_uint32 idx, lvalue* lv_out);

#endif