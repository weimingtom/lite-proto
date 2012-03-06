#include "lib_lp.h"
#include "lp_conf.h"
#include "lib_mes.h"
#include "lib_io.h"

static int llp_reg_mes_value(llp_env* env, t_reg_mes* rmp, slice* sl);
static int llp_read_message(llp_env* env, char** out_name, slice* sl);

static int get_llp_env(llp_env* env_p)
{
	check_null(env_p, LP_FAIL);
	check_fail(lib_table_new(&env_p->mes, DEF_MES_LEN, reg_mes), LP_FAIL);
	check_fail(lib_table_new(&env_p->dmes, DEF_DMES_LEN, def_mes), LP_FAIL);
	memset(&env_p->mesN, 0, sizeof(env_p->mesN));
	return LP_TRUE;
}

static void free_llp_env(llp_env* p)
{
	if(p)
	{
		lib_table_free(&p->dmes);
		lib_table_free(&p->mes);
		lib_Stable_free(&p->mesN);
	}
}

int llp_del_mes(llp_env* env, char* mes_name)
{
	struct _nl* mp = NULL;
	struct _lp_value* lv = NULL;
	check_null(env, LP_FAIL);
	check_null(env, LP_FAIL);
	check_null(lv=lib_table_look(&env->mes, mes_name), LP_FAIL);
	mp = lv->value.reg_mesV.mNs;
	while(mp)
	{
		lib_table_del(&env->dmes, mp->name);
		mp = mp->next;
	}
	lib_table_del(&env->mes, mes_name);

	return LP_TRUE;
}

int llp_reg_mes(llp_env* env, char* mes_name)
{
	slice sl = {0};
	lp_value* lv = NULL;
	f_handle fd = f_open(mes_name, "r");
	sl.sp_size = fsize(fd);
	check_null(env, LP_FAIL);
	check_null(mes_name, LP_FAIL);
	check_null(fd, LP_FAIL);
	
	sl.sp = (byte*)malloc(sl.sp_size);
	sl.b_sp = sl.sp;
	f_read(sl.sp, 1, sl.sp_size, fd);
	f_close(fd);
	if( (lv=(lp_value*)lib_table_add(&env->mes, lib_Stable_add(&env->mesN, mes_name)))!= NULL )
	{
		check_fail( llp_reg_mes_value(env, &lv->value.reg_mesV, &sl), 
					(llp_del_mes(env, mes_name), free(sl.b_sp), LP_FAIL)
				  );
	}

	free(sl.b_sp);
	return LP_TRUE;
}

static int llp_reg_mes_value(llp_env* env, t_reg_mes* rmp, slice* sl)
{
	char* out_name = NULL;
	check_null(rmp, LP_FAIL);
	check_null(sl, LP_FAIL);
	check_null(sl->sp, LP_FAIL);

	for(;;)
	{
		switch(llp_read_message(env, &out_name, sl))
		{
		case LP_TRUE:
			{
				struct _nl* nl = (struct _nl*)malloc(sizeof(struct _nl));
				nl->name = out_name;
				nl->next = rmp->mNs;
				rmp->mNs = nl;
				rmp->mNs_count++;
			}
			break;
		case LP_FAIL:
			// des
			return LP_FAIL;
		case LP_END:
			goto RMV_END;
		}
	}
RMV_END:
	return LP_TRUE;
}

static int llp_read_filed(llp_env* env, t_def_mes* des_mes, slice* sl)
{
	unsigned int i=0;
	des_mes->message_tfl = (t_Mfield*)malloc(sizeof(t_Mfield)*des_mes->message_count);
	memset(des_mes->message_tfl, 0, sizeof(t_Mfield)*des_mes->message_count);
	check_fail(lib_table_new(&des_mes->message_filed, des_mes->message_count, def_field), LP_FAIL);

	for(i=0; i<des_mes->message_count; i++)
	{
		char* f_name = NULL;
		struct _lp_value* lv = NULL;
		check_fail(sl_Rbyte(sl, &des_mes->message_tfl[i].tag), LP_FAIL);
		if(tag_type(des_mes->message_tfl[i].tag) == lpt_message)
		{
			char* fms = NULL;
			struct _lp_value* lv = NULL;
			check_fail(sl_Rstr(sl, &fms), LP_FAIL);
			lv=lib_table_look(&env->dmes, fms);
			if(lv == NULL)
			{
				lv = lib_table_add(&env->dmes, lib_Stable_add(&env->mesN, fms));
				check_null(lv, LP_FAIL);
			}
			des_mes->message_tfl[i].tms = &lv->value.def_mesV;
		}
		
		check_fail(sl_Rstr(sl, &f_name), LP_FAIL);
		check_null(lv=lib_table_add(&des_mes->message_filed, lib_Stable_add(&env->mesN, f_name)), LP_FAIL);
		lv->value.def_fieldV.f_id = i;
	}

	return LP_TRUE;
}

static int llp_read_message(llp_env* env, char** out_name, slice* sl)
{
	lp_value* lds = NULL;
	
	check_sl(sl);
	check_fail(sl_Rstr(sl, out_name), LP_FAIL);									// read message name
	*out_name = lib_Stable_add(&env->mesN, *out_name);							// add name
	check_null(lds=(lp_value*)lib_table_add(&env->dmes, *out_name), LP_FAIL);	// begin add message body
	check_fail(sl_Ruint(sl, &(lds->value.def_mesV.message_id)), LP_FAIL);			// read id
	check_fail(sl_Ruint(sl, &(lds->value.def_mesV.message_count)), LP_FAIL);		// read message count
	check_fail(llp_read_filed(env, &lds->value.def_mesV, sl), LP_FAIL);			// read filed

	return LP_TRUE;
}


// API
llp_env* llp_new_env()
{
	llp_env* ret = NULL;
	check_null(ret=(llp_env*)malloc(sizeof(llp_env)), NULL);
	check_fail(get_llp_env(ret), (free(ret), NULL));
	
	return ret;
}


void llp_free_env(llp_env* p)
{
	if(p)
	{
		free_llp_env(p);
		free(p);
	}
}
