
#include "llp.h"
#include "llp_lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lp_conf.h"
#include "lib_table.h"
#include "lib_stringpool.h"
#include <stdlib.h>
#include <windows.h>



void dump_map(llp_map* l_map)
{
	size_t i=0;
	for(i=0; i<l_map->size; i++)
	{
		print("map[%d]{hash=%d hash_full=%d key=%s next=%d}\n", i+1, l_map->table[i].hash % l_map->size +1, 
				l_map->table[i].hash, l_map->table[i].key, l_map->table[i].next);
	}	
}


int test_map(void)
{
	int i=0;
	llp_map* l_map =  lib_map_new();
	llp_kv kv = {0};
//	char buff[24] = {0};
	char* buff[] = {
		"946",
		"15273",
		"9314",
		"12367",
		"4720",
		"4551",
		"15424",
		"13894",
		"17172",
		"13947"
	};
	srand(GetTickCount());
	for(i=0; i<sizeof(buff)/sizeof(char*); i++)
	{
		int number = rand();
	/*	memset(buff, 0, sizeof(buff));
		itoa(number, buff, 10);
	*/	kv.key = (buff[i]);
		kv.vp = (void*)number;
		lib_map_add(l_map, &kv);
	}

	dump_map(l_map);
	lib_map_free(l_map);
	print_mem();
	return 0;
}


void test_stringpool()
{
	string_pool* str_pool =  lib_stringpool_new();
	int i=0;
	char buff[24] = {0};
	srand(GetTickCount());
	for(i=0; i<10; i++)
	{
		int number = rand();
		itoa(number, buff, 10);
		lib_stringpool_add(str_pool, buff);
	}

	dump_stringpool(str_pool);
	dump_map( *((llp_map**)(((size_t)str_pool)+4)) );
	lib_stringpool_free(str_pool);
}

#define LENSS 1000000

void test_llp(llp_env* env)
{
	int i=0;
	unsigned long tt[2] = {0};
	llp_mes* lm = llp_message_new(env, "test");
	llp_mes* lms = llp_message_new(env, "test");
	llp_mes* temp = NULL;

	tt[0] = GetTickCount();
	print("begin time: %ld\n", tt[0]);
	for(i=0; i<LENSS; i++)
	{
		llp_Wmes_int32(lm, "aa", 123);
		llp_Wmes_int32(lm, "bb", 456);
		llp_Wmes_string(lm, "cc", "hi, I am string!");

		llp_in_message(llp_out_message(lm), lms);
		llp_message_clr(lm);
		llp_message_clr(lms);
	}		
	tt[1] = GetTickCount();
	print("end time: %ld\n all time[lens = %ld]: %ld\n", tt[1], LENSS, tt[1]-tt[0]);
	llp_message_free(lms);
	llp_message_free(lm);
}




void dump_lvaue(lvalue* lv, int tab)
{
	int i=0;
	static char* type_s[] = {
		"int32",
		"int64",
		"float32",
		"float64",
		"string",
		"stream",
		"message"
	};

	static char* is_rep[] = {
		" ",
		"[0]"
	};
	
	for(i=0; i<tab; i++) print(" ");
	switch(lv->t)
	{
	case LLPT_INT32:
		print("%s %s%s = %d\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.i32);
		break;
	case  LLPT_INT64:
		print("%s %s%s = %ld\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.i64);
		break;
	case  LLPT_FLOAT32:
		print( "%s %s%s = %f\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.f32);
		break;
	case LLPT_FLOAT64:
		print( "%s %s%s = %lf\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.i64);
		break;
	case LLPT_STRING:
		print( "%s %s%s = %s\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.str);
		break;
	case LLPT_STREAM:
		print( "%s %s%s = %p\n", type_s[lv->t], lv->filed_name, is_rep[lv->is_repeated], lv->v.stream);
		break;
	case LLPT_MESSAGE:
		print("%s %s%s = %p\n", lv->v.mes.mes_name, lv->filed_name, is_rep[lv->is_repeated], lv->v.mes.lm);
		{
			int idx=1;
			lvalue llv={0};
			while(idx=llp_next(lv->v.mes.lm, idx, &llv))
				dump_lvaue(&llv, tab+2);
		}
		break;
	default:
		print("fail lvalue!\n");
		break;
	}

}



void test_llp_next(llp_env* env)
{
	llp_mes* lms;
	byte buffer[] = {1,2,3,4,5,6};
	llp_uint32 idx =1;
	lvalue lv ={0};
	llp_mes* lm = llp_message_new(env, "test");
	slice sl;
	sl.b_sp = sl.sp = buffer;
	sl.sp_size = sizeof(buffer);

	llp_Wmes_int32(lm, "aa", 123);
	llp_Wmes_int32(lm, "aa", 1111);
	llp_Wmes_float32(lm, "cc", (float)6.797);
	llp_Wmes_int64(lm, "bb", 456);
	//llp_Wmes_float64(lm, "dd", 1.2234);
	llp_Wmes_string(lm, "ee", "test string!");
	llp_Wmes_stream(lm, "ff", buffer, sizeof(buffer));

	lms=llp_Wmes_message(lm, "io");
	llp_Wmes_int32(lms, "ia", 8888);
	llp_Wmes_string(lms, "ib", "i am info message!");

	while(idx=llp_next(lm, idx, &lv))
	{
		dump_lvaue(&lv, 0);
	}

	llp_message_free(lm);
}


int main(void)
{
//	test_stringpool();
	int ret = 0;
	lua_State* L = lua_open();
	luaopen_base(L);
	luaopen_string(L);
	luaopen_table(L);
	llpL_open(L, NULL);

	if(luaL_dofile(L, "test.lua"))
	{
		printf("<error>: %s\n", lua_tostring(L, -1));
	}

// 	llp_env* env = llp_new_env();
// 
// 	ret = llp_reg_mes(env, "F:\\code\\llp_branches\\llp1_1obj\\test.mes.lpb");
// 
// 	test_llp_next(env);
// 
// //	test_llp(env);
// 
// 	llp_free_env(env);
	
	print("call here!\n");
	lua_close(L);
	print_mem(); 
	return 0;
}