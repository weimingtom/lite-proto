#ifndef _LLP_H_
#define _LLP_H_
#include "lp_conf.h"


typedef struct _llp_env llp_env;
typedef struct _llp_mes llp_mes;
// --------env
llp_env* llp_new_env();
void llp_free_env(llp_env* p);

// --------registration message from .lpb files
int llp_reg_mes(llp_env* env, char* lpb_file_name);
// --------registration message from slice
int llp_reg_Smes(llp_env* env, slice* buff);

// --------new/delete a message object
llp_mes*  llp_message_new(llp_env* env, char* mes_name);
void llp_message_clr(llp_mes* in_mes);
void  llp_message_free(llp_mes* in_mes);

// --------write a message object
int llp_Wmes_integer(llp_mes* lm, char* filed_name, llp_integer number);
int llp_Wmes_real(llp_mes* lm, char* filed_name,	llp_real number);
int llp_Wmes_string(llp_mes* lm, char* filed_name, char* str);
int llp_Wmes_bytes(llp_mes* lm, char* filed_name, slice* sl);
llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_name);

// ---------read a message object
llp_integer llp_Rmes_integer(llp_mes* lm, char* filed_name, unsigned int al_inx);
llp_real	llp_Rmes_real(llp_mes* lm, char* filed_name, unsigned int al_inx);
slice* llp_Rmes_bytes(llp_mes* lm, char* filed_name, unsigned int al_inx);
char* llp_Rmes_string(llp_mes* lm, char* filed_name, unsigned int al_inx);
llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_name, unsigned int al_inx);
llp_uint32 llp_Rmes_size(llp_mes* lm, char* filed_name);

// ------- out/in a message body
slice* llp_out_message(llp_mes* lms);
int llp_in_message(slice* in, llp_mes* lms);

#endif
