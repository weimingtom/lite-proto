#ifndef  _LIB_MES_H_
#define  _LIB_MES_H_
#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_al.h"


typedef struct _llp_mes{
	t_def_mes*	 d_mes;			// message body struct
	llp_array*   filed_al;		// filed data
	size_t		 filed_lens;

	slice		 sio;			// out 
}llp_mes;

llp_mes*  llp_message_new(llp_env* env, char* mes_name);
void llp_message_clr(llp_mes* in_mes);
void  llp_message_free(llp_mes* in_mes);

int llp_Wmes_int32(llp_mes* lm, char* filed_str, int number);
int llp_Wmes_int64(llp_mes* lm, char* filed_str, long number);
int llp_Wmes_float32(llp_mes* lm, char* filed_str, float number);
int llp_Wmes_float64(llp_mes* lm, char* filed_str, double number);
int llp_Wmes_string(llp_mes* lm, char* filed_str, char* str);
llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_str);


int llp_Rmes_int32(llp_mes* lm, char* filed_str, unsigned int al_inx);
long llp_Rmes_int64(llp_mes* lm, char* filed_str, unsigned int al_inx);
float llp_Rmes_float32(llp_mes* lm, char* filed_str, unsigned int al_inx);
double llp_Rmes_float64(llp_mes* lm, char* filed_str, unsigned int al_inx);
char* llp_Rmes_string(llp_mes* lm, char* filed_str, unsigned int al_inx);
llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_str, unsigned int al_inx);

#endif