#include "lib_lp.h"
#include "lp_conf.h"
#include "lib_mes.h"
#include "lib_io.h"

static int llp_reg_mes_value(llp_env* env, t_reg_mes* rmp, slice* sl);
static int llp_read_message(llp_env* env, char** out_name, slice* sl);
int llp_reg_mes(llp_env* env, char* mes_name);
int llp_del_mes(llp_env* env, char* mes_name);

#define LENS 4

void test2_p_mes(llp_mes* lpm)
{
	int i=0;
	print("test2 id= %d \n", llp_Rmes_int64(lpm, "id", 0));
	print("test2 name = %s \n", llp_Rmes_string(lpm, "name", 0));
	print("test2 rgb = %d \n", llp_Rmes_int32(lpm, "name", 0));
	for(i=0; i<LENS; i++)
		print("test2 al[%d] = %d\n", i, llp_Rmes_int32(lpm, "al", i));
	{
		llp_mes* nms = NULL;
		for(i=0; i<4; i++)
		{
			nms = llp_Wmes_message(lpm, "i_al");
			llp_Wmes_int32(nms, "al", i+10);
			llp_Wmes_int32(nms, "b", i+100);
		}
		for(i=0; i<4; i++)
		{
			nms = llp_Rmes_message(lpm, "i_al", i);
			print("info al[%d]= %d\n", i, llp_Rmes_int32(nms, "al", 0));
			print("info b[%d] = %d\n", i, llp_Rmes_int32(nms, "b", 0));
		}
	}
}

int main(void)
{
	slice ins = {0};
	int i=0;
	llp_env env = {0};
	llp_mes* lpm = NULL;
	llp_mes* lpm2 = NULL;

	get_llp_env(&env);
	
	print("----reg before mem = %d long[%d] int[%d] float[%d]  double[%d]\n", mem, sizeof(llp_int64), sizeof(llp_int32), sizeof(float), sizeof(double));
	llp_reg_mes(&env, "test.lpb");
	print("-----reg after mem = %d\n", mem);
	
	// test1 message 
	lpm = llp_message_new(&env, "test1");		// create a message obj
	llp_Wmes_int32(lpm, "a", 1523698);
	llp_Wmes_int32(lpm, "b", 456);
	llp_Wmes_string(lpm, "name", "test_name~~");
	
	print("test1 a= %d \n", llp_Rmes_int32(lpm, "a", 0));
	print("test1 b = %d \n", llp_Rmes_int32(lpm, "b", 0));
	print("test1 name = %s \n", llp_Rmes_string(lpm, "name", 0));
//	llp_out_message(lpm);
	llp_message_free(lpm);

	

	// test2 message
	lpm = llp_message_new(&env, "test2");
	lpm2 = llp_message_new(&env, "test2");
	llp_Wmes_int64(lpm, "id", 333);
	llp_Wmes_string(lpm, "name", "t2_name!");
	llp_Wmes_int32(lpm, "rgb", 444);
	for(i=0; i<LENS; i++)
		llp_Wmes_int32(lpm, "al", i+10);
	test2_p_mes(lpm);
	
	llp_out_message(lpm);
	ins.sp = lpm->sio.b_sp;
	ins.b_sp = lpm->sio.b_sp;
	ins.sp_size = lpm->sio.sp - lpm->sio.b_sp;
	llp_in_message(&ins, lpm2);
	print("in-------\n");
	test2_p_mes(lpm2);
	llp_message_free(lpm);

	free_llp_env(&env);
	print("mem = %d\n", mem);
	return 0;
}

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
	if( lv=(lp_value*)lib_table_add(&env->mes, lib_Stable_add(&env->mesN, mes_name)) )
		check_fail(llp_reg_mes_value(env, &lv->value.reg_mesV, &sl), (llp_del_mes(env, mes_name), LP_FAIL));

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
	rmp->mes_size = sl->sp_size;
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
			check_null(lv=lib_table_look(&env->dmes, fms), LP_FAIL);
			des_mes->message_tfl[i].tms = &lv->value.def_mesV;
		}
		
		check_fail(sl_Rstr(sl, &f_name), LP_FAIL);
		check_null(lv=lib_table_add(&des_mes->message_filed, f_name), LP_FAIL);
		lv->value.def_fieldV.f_id = i;
	}

	return LP_TRUE;
}

static int llp_read_message(llp_env* env, char** out_name, slice* sl)
{
	lp_value* lds = NULL;
	
	check_sl(sl);
	check_fail(sl_Rstr(sl, out_name), LP_FAIL);									// read message name
	check_null(lds=(lp_value*)lib_table_add(&env->dmes, *out_name), LP_FAIL);	// begin add message body
	check_fail(sl_Ruint(sl, &lds->value.def_mesV.message_id), LP_FAIL);			// read id
	check_fail(sl_Ruint(sl, &lds->value.def_mesV.message_count), LP_FAIL);		// read message count
	check_fail(llp_read_filed(env, &lds->value.def_mesV, sl), LP_FAIL);			// read filed

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