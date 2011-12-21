#include "lib_mes.h"
#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_table.h"
#include "lib_io.h"

void llp_message_freeV(llp_value* lp_v);

llp_mes* _llp_message_new(t_def_mes* def_mesP)
{
	size_t i=0;
	llp_mes* out_mes = NULL;
	check_null(def_mesP, NULL);
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
llp_mes*  llp_message_new(llp_env* env, char* mes_name)
{
	struct _lp_value* lv = NULL;
	check_null(env, NULL);
	check_null(mes_name, NULL);

	check_null(lv=lib_table_look(&env->dmes, mes_name), NULL);
	return _llp_message_new(&lv->value.def_mesV);
}

void llp_message_clr(llp_mes* in_mes)
{
	size_t i=0;
	if(in_mes == NULL)
		return;

	for(i=0; i<in_mes->filed_lens; i++)
	{
		if(in_mes->d_mes->message_tfl[i].tag == lpt_message)
			lib_array_clr(&in_mes->filed_al[i], llp_message_freeV);
		else
			lib_array_clr(&in_mes->filed_al[i], NULL);
	}
	llp_out_close(&in_mes->sio);
}

void  llp_message_free(llp_mes* in_mes)
{
	size_t i=0;
	if(in_mes==NULL)
		return;

	for(i=0; i<in_mes->filed_lens; i++)
	{
		if(tag_type(in_mes->d_mes->message_tfl[i].tag)==lpt_message)
			lib_array_free(&in_mes->filed_al[i], llp_message_freeV);
		else
			lib_array_free(&in_mes->filed_al[i], NULL);
	}
	free(in_mes->filed_al);
	llp_out_close(&in_mes->sio);
	free(in_mes);
}

void llp_message_freeV(llp_value* lp_v)
{
	llp_message_free(lp_v->lp_mes);
}

int _llp_Wmes(llp_mes* lm, int inx, byte v_type, void* msd)
{
	llp_value lpv = {0};
	byte tag = 0;
	tag = lm->d_mes->message_tfl[inx].tag;
	
	// error : if a filed is not repeated, so dobule add is error
	if( (tag_type(tag)!=v_type)||(tag_state(tag) == lpf_req && lm->filed_al[inx].lens>=1) )
		return LP_FAIL;
	
	switch(tag_type(tag))
	{
	case lpt_int32:
		lpv.lp_int32 = *((llp_int32*)msd);
		break;
	case lpt_int64:
		lpv.lp_int64 = *((llp_int64*)msd);
		break;
	case  lpt_float32:
		lpv.lp_float32 = *((llp_float32*)msd);
		break;
	case  lpt_float64:
		lpv.lp_float64 = *((llp_float64*)msd);
		break;
	case  lpt_string:
		lpv.lp_str = (char*)msd;
		break;
	case lpt_message:
		//	lpv.lp_mes = (llp_mes*)msd;
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
	int inx = 0;
	struct _lp_value* lv = NULL;
	
	check_null(lm, LP_FAIL);
	check_null(filed_str, LP_FAIL);
	check_null(msd, LP_FAIL);
	check_null(lv=lib_table_look(&lm->d_mes->message_filed, filed_str), LP_FAIL);
	inx = lv->value.def_fieldV.f_id;

	return _llp_Wmes(lm, inx, v_type, msd);
}

int llp_Wmes_int32(llp_mes* lm, char* filed_str, llp_int32 number)
{
	check_fail(llp_Wmes(lm, filed_str, lpt_int32, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

int llp_Wmes_int64(llp_mes* lm, char* filed_str, llp_int64 number)
{
	check_fail(llp_Wmes(lm, filed_str, lpt_int64, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

int llp_Wmes_float32(llp_mes* lm, char* filed_str, llp_float32 number)
{
	check_fail(llp_Wmes(lm, filed_str, lpt_float32, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

int llp_Wmes_float64(llp_mes* lm, char* filed_str, llp_float64 number)
{
	check_fail(llp_Wmes(lm, filed_str, lpt_float64, (void*)(&number)), LP_FAIL);
	return LP_TRUE;
}

int llp_Wmes_string(llp_mes* lm, char* filed_str, char* str)
{
	check_fail(llp_Wmes(lm, filed_str, lpt_string, (void*)str), LP_FAIL);
	return LP_TRUE;
}

llp_mes* llp_Wmes_message(llp_mes* lm, char* filed_str)
{
	llp_mes* lms = NULL;
	check_fail(llp_Wmes(lm, filed_str, lpt_message, (void*)(&lms)), NULL);
	return lms;
}

static llp_value* llp_Rmes(llp_mes* lm, char* filed_str, byte v_type, unsigned int al_inx)
{
	int inx = 0;
	byte tag = 0;
	struct _lp_value* lv = NULL;

	check_null(lm, NULL);
	check_null(filed_str, NULL);
	check_null(lv=lib_table_look(&lm->d_mes->message_filed, filed_str), NULL);
	inx = lv->value.def_fieldV.f_id;
	tag = lm->d_mes->message_tfl[inx].tag;

	if(tag_type(tag)!=v_type)
		return NULL;

	switch(tag_type(tag))
	{
	case lpt_int32:
	case lpt_int64:
	case  lpt_float32:
	case  lpt_float64:
	case  lpt_string:
	case lpt_message:
		break;
	default:
		return NULL;
	}

	return lib_array_inx(&lm->filed_al[inx], al_inx);
}

llp_int32 llp_Rmes_int32(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_int32, al_inx), 0);
	
	return lpv->lp_int32;
}

llp_int64 llp_Rmes_int64(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_int64, al_inx), 0);
	
	return lpv->lp_int64;
}

llp_float32 llp_Rmes_float32(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_float32, al_inx), 0.0);
	
	return lpv->lp_float32;
}

llp_float64 llp_Rmes_float64(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_float64, al_inx), 0.0);
	
	return lpv->lp_float64;
}

char* llp_Rmes_string(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_string, al_inx), NULL);
	
	return lpv->lp_str;
}

llp_mes* llp_Rmes_message(llp_mes* lm, char* filed_str, unsigned int al_inx)
{
	llp_value* lpv = NULL;
	check_null(lpv=llp_Rmes(lm, filed_str, lpt_message, al_inx), NULL);
	
	return lpv->lp_mes;
}

unsigned int llp_Rmes_size(llp_mes* lm, char* filed_str)
{
	struct _lp_value* lv = NULL;
	int inx = 0;
	check_null(lm, 0);
	check_null(filed_str, 0);
	check_null(lv=lib_table_look(&lm->d_mes->message_filed, filed_str), 0);
	inx = lv->value.def_fieldV.f_id;

	return lm->filed_al[inx].lens;
}