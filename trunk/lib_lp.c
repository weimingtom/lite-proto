#include "lib_lp.h"
#include "lp_conf.h"

static int llp_reg_mes_value(llp_env* env, t_reg_mes* rmp, slice* sl);
static int llp_read_message(llp_env* env, char** out_name, slice* sl);


#define  sl_emp(sl)		( ((sl)->sp)-((sl)->b_sp) )
#define check_sl(sl)	do{ if( (sl)==NULL || (sl)->sp==NULL || ((unsigned int)((sl)->sp-(sl)->b_sp))>(sl)->sp_len ) return LP_FAIL;}while(0)
int sl_Rstr(slice* sl, char** out)
{
	check_sl(sl);
	*out = (char*)(sl->sp);
	sl->sp += (strlen(*out)+1);
	check_sl(sl);

	return LP_TRUE;
}	

int  sl_Ruint(slice* sl, size_t* out)
{
	check_sl(sl);
	*out = *((size_t*)(sl->sp));
	sl->sp += sizeof(size_t);
	check_sl(sl);

	return LP_TRUE;
}

int main(void)
{
	FILE* fp = fopen("test.lpb", "r"); 
	long lens = fsize(fp);
	llp_env env = {0};

	check_null(fp, LP_FAIL);
	get_llp_env(&env);

	free_llp_env(&env);
	return 0;
}

 int _free_s(void* p)
{
	 free(p);
	return  LP_TRUE;
}

static int get_llp_env(llp_env* env_p)
{
	check_null(env_p, LP_FAIL);
	check_fail(lib_table_new(&env_p->mes, DEF_MES_LEN), LP_FAIL);
	memset(&env_p->mesN, 0, sizeof(env_p->mesN));
	return LP_TRUE;
}

static void free_llp_env(llp_env* p)
{
	if(p)
	{
		lib_table_free(&p->mes);
		lib_Stable_free(&p->mesN);
		free(p);
	}
}


int llp_reg_mes(llp_env* env, char* mes_name)
{
	slice sl = {0};
	lp_value* lv = NULL;
	f_handle fd = f_open(mes_name, "r");
	sl.sp_len = fsize(fd);
	check_null(env, LP_FAIL);
	check_null(mes_name, LP_FAIL);
	check_null(fd, LP_FAIL);
	
	sl.sp = (byte*)malloc(sl.sp_len);
	sl.b_sp = sl.sp;
	f_read(sl.sp, 1, sl.sp_len, fd);
	if( lv=(lp_value*)lib_table_add(&env->mes, lib_Stable_add(&env->mesN, mes_name)) )
	{
		lv->type =reg_mes;
		llp_reg_mes_value(env, &lv->value.reg_mesV, &sl);
	}
	f_close(fd);
	return LP_TRUE;
}

static int llp_reg_mes_value(llp_env* env, t_reg_mes* rmp, slice* sl)
{
	char* out_name = NULL;
	check_null(rmp, LP_FAIL);
	check_null(sl, LP_FAIL);
	check_null(sl->sp, LP_FAIL);

	rmp->mes_p = sl->sp;
	rmp->mes_size = sl->sp_len;
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

static int llp_read_filed()
{

}

static int llp_read_message(llp_env* env, char** out_name, slice* sl)
{
	lp_value* lds = NULL;
	int mes_len = strlen(sl->sp);
	if(mes_len > sl_emp(sl))
		return LP_FAIL;

	check_fail(sl_Rstr(sl, out_name), LP_FAIL);							// read message name
	check_null(lds=(lp_value*)lib_table_add(&env->dmes, *out_name), LP_FAIL);		//  begin add message body
	lds->type = def_mes;
	check_fail(sl_Ruint(sl, &lds->value.def_mesV.message_id), LP_FAIL);						// read id
//	lds->value.def_mesV.message_filed

	return LP_TRUE;
}


// API
void* new_env()
{
	llp_env* ret = NULL;
	check_null(ret=(llp_env*)malloc(sizeof(llp_env)), NULL);
	check_fail(get_llp_env(ret), (free(ret), NULL));
	
	return ret;
}

void free_env(void* p)
{
	free_llp_env((llp_env*) p);
}