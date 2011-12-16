#include "lib_mes.h"
#include "lp_conf.h"
#include "lib_lp.h"
#include "lib_table.h"

// create a  message object
int	llp_message(llp_env* env, char* mes_name, llp_mes* out_mes)
{
	struct _lp_value* lv = NULL;
	check_null(env, LP_FAIL);
	check_null(mes_name, LP_FAIL);
	check_null(out_mes, LP_FAIL);
	
	check_null(lv=lib_table_look(&env->dmes, mes_name), LP_FAIL);
	out_mes->d_mes = &lv->value.def_mesV;
	out_mes->filed_lens = lv->value.def_mesV.message_count;
	out_mes->filed_V = (filed_value*)malloc(sizeof(filed_value)*out_mes->filed_lens);
	memset(out_mes->filed_V, 0, sizeof(filed_value)*out_mes->filed_lens);

	return LP_TRUE;
}

// write a int at a message obj
int llp_Wmes_int32(llp_mes* lm, char* filed_str, int number)
{
	int inx = 0;
	byte tag = 0;
	struct _lp_value* lv = NULL;
	check_null(lm, LP_FAIL);
	check_null(filed_str, LP_FAIL);
	check_null(lv=lib_table_look(&lm->d_mes->message_filed, filed_str), LP_FAIL);
	inx = lv->value.def_fieldV.f_id;
	tag = lm->d_mes->message_tfl[inx].tag;
	if(tag_type(tag) == lpt_int32 && tag_state(tag) == lpf_req )
	{
//		lm->filed_V[inx].
	}
	else
		return LP_FAIL;
	return LP_TRUE;
}