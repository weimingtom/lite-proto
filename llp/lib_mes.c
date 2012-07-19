#include "lib_mes.h"
#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_table.h"
#include "lib_io.h"

void llp_message_freeV(llp_value* lp_v);
void llp_string_freeV(llp_value* lp_v);
void llp_bytes_freeV(llp_value* lp_v);
char* malloc_string(char* str);
slice* malloc_slice(slice* sl);

llp_mes* _llp_message_new(t_def_mes* def_mesP)
{
	llp_uint32 i=0;
	llp_mes* out_mes = NULL;
	check_null(def_mesP, NULL);
	check_null(def_mesP->message_filed, NULL);
	out_mes = (llp_mes*)malloc(sizeof(llp_mes));
	memset(out_mes, 0, sizeof(llp_mes));
	
	out_mes->d_mes = def_mesP;
	out_mes->filed_lens = def_mesP->message_count;
	out_mes->filed_al = (llp_array*)malloc(sizeof(llp_array)*out_mes->filed_lens);
	for(i=0; i<out_mes->filed_lens; i++)
		lib_array_new(&out_mes->filed_al[i]);
	
	return out_mes;
}

// create a  message object
LLP_API llp_mes*  llp_message_new(llp_env* env, char* mes_name)
{
	t_def_mes* mes_p= NULL;
	check_null(env, NULL);
	check_null(mes_name, NULL);

	check_null(mes_p=lib_Mmap_find(env->dmes, mes_name), NULL);
	return _llp_message_new(mes_p);
}

typedef void (*lib_array_cf_func)(llp_array* al, al_free free_func);
int _llp_message_cf(llp_mes* in_mes, lib_array_cf_func cf_func)
{
	size_t i=0;
	if(in_mes == NULL)
		return LP_FAIL;
	
	for(i=0; i<in_mes->filed_lens; i++)
	{
		switch(tag_type(in_mes->d_mes->message_tfl[i].tag))
		{
		case t_Kmessage:
			cf_func(&in_mes->filed_al[i], llp_message_freeV);
			break;
		case t_kstring:
			cf_func(&in_mes->filed_al[i], llp_string_freeV);
			break;
		case t_Kbytes:
			cf_func(&in_mes->filed_al[i], llp_bytes_freeV);
			break;
		default:
			cf_func(&in_mes->filed_al[i], NULL);
			break;
		}
	}

	return LP_TRUE;
}

LLP_API void llp_message_clr(llp_mes* in_mes)
{
	if(_llp_message_cf(in_mes, lib_array_clr)==LP_FAIL)
		return;
	llp_out_clr(&in_mes->sio);
}

LLP_API void  llp_message_free(llp_mes* in_mes)
{
	if(_llp_message_cf(in_mes, lib_array_free)==LP_FAIL)
		return;
	free(in_mes->filed_al);
	llp_out_close(&in_mes->sio);
	free(in_mes);
}

void llp_message_freeV(llp_value* lp_v)
{
	llp_message_free(lp_v->lp_mes);
}

void llp_string_freeV(llp_value* lp_v)
{
	if(lp_v->lp_str)
		free(lp_v->lp_str);
}

void llp_bytes_freeV(llp_value* lp_v)
{
	if(lp_v->lp_bytes){
		free(lp_v->lp_bytes->b_sp);
		free(lp_v->lp_bytes);
	}
}

int _llp_Wmes(llp_mes* lm, int inx, byte v_type, void* msd)
{
	llp_value lpv = {0};
	byte tag = 0;
	tag = lm->d_mes->message_tfl[inx].tag;
	
	// error : if a filed is not repeated, so double add is error
	if( (tag_type(tag)!=v_type) || (tag_state(tag)==lpf_req &&  lm->filed_al[inx].lens>=1) )
		return LP_FAIL;
	
	switch(tag_type(tag))
	{
	case t_Kinteger:
		lpv.lp_integer = *((llp_integer*)msd);
		break;
	case t_Kreal:
		lpv.lp_real = *((llp_real*)msd);
		break;
	case t_kstring:
		lpv.lp_str = malloc_string((char*)msd);
		break;
	case t_Kbytes:
		lpv.lp_bytes = malloc_slice((slice*)msd);
		break;
	case t_Kmessage:
		{
			lpv.lp_mes = _llp_message_new(lm->d_mes->message_tfl[inx].tms);
			*((llp_mes**)msd) = lpv.lp_mes;
		}
		break;
	default:
		return LP_FAIL;
	}

	lib_array_add(&lm->filed_al[inx], &lpv);	
	return LP_TRUE;
}

// write a int at a message obj
static int llp_Wmes(llp_mes* lm, char* filed_str, byte v_type, void* msd)
{
	int* id_p = NULL;

	check_null(lm, LP_FAIL);
	check_null(filed_str, LP_FAIL);
	check_null(msd, LP_FAIL);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_filed, filed_str), LP_FAIL);

	return _llp_Wmes(lm, *id_p, v_type, msd);
}

LLP_API int llp_Wmes_bytes(llp_mes* lm, char* filed_name, slice* sl)
{	
	check_fail(llp_Wmes(lm, filed_name, t_Kbytes, (void*)sl), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_integer(llp_mes* lm, char* filed_name, llp_integer number)
{
	check_fail(llp_Wmes(lm, filed_name, t_Kinteger, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_real(llp_mes* lm, char* filed_name, llp_real number)
{
	check_fail(llp_Wmes(lm, filed_name, t_Kreal, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

LLP_API int llp_Wmes_string(llp_mes* lm, char* filed_name, char* str)
{
	check_fail(llp_Wmes(lm, filed_name, t_kstring, (void*)str), LP_FAIL);
	return LP_TRUE;
}

LLP_API llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_name)
{
	llp_mes* lms = NULL;
	check_fail(llp_Wmes(lm, filed_name, t_Kmessage, (void*)(&lms)), NULL);
	return lms;
}

static llp_value* llp_Rmes(llp_mes* lm, char* filed_str, byte v_type, unsigned int al_inx)
{
	int inx = 0;
	int* id_p = NULL;
	byte tag = 0;

	check_null(lm, NULL);
	check_null(filed_str, NULL);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_filed, filed_str), NULL);
	inx = *id_p;
	tag = lm->d_mes->message_tfl[inx].tag;

	if(tag_type(tag)!=v_type)
		return NULL;

	switch(tag_type(tag))
	{
	case t_Kinteger:
	case t_Kreal:
	case t_kstring:
	case t_Kmessage:
	case t_Kbytes:
		break;
	default:
		return NULL;
	}

	return lib_array_inx(&lm->filed_al[inx], al_inx);
}

LLP_API slice* llp_Rmes_bytes(llp_mes* lm, char* filed_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_name, t_Kbytes, al_inx), NULL);
	
	return lpv->lp_bytes;
}

LLP_API llp_integer llp_Rmes_integer(llp_mes* lm, char* filed_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_name, t_Kinteger, al_inx), (llp_integer)0);
	
	return lpv->lp_integer;
}

LLP_API llp_real llp_Rmes_real(llp_mes* lm, char* filed_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_name, t_Kreal, al_inx), (llp_real)0.0);
	
	return lpv->lp_real;
}

LLP_API char* llp_Rmes_string(llp_mes* lm, char* filed_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_name, t_kstring, al_inx), NULL);
	
	return lpv->lp_str;
}

LLP_API llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_name, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_name, t_Kmessage, al_inx), NULL);
	
	return lpv->lp_mes;
}

LLP_API llp_uint32 llp_Rmes_size(llp_mes* lm, char* filed_name)
{
	int* id_p = NULL;

	check_null(lm, 0);
	check_null(filed_name, 0);
	check_null(id_p=lib_Fmap_find(lm->d_mes->message_filed, filed_name), 0);

	return lm->filed_al[*id_p].lens;
}

