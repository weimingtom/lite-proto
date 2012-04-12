
#include "llp.h"
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


int main(void)
{
//	test_stringpool();
	int ret = 0;
	llp_env* env = llp_new_env();
	ret = llp_reg_mes(env, "test.mes.lpb");

	test_llp(env);

	llp_free_env(env);

	print_mem(); 
	getchar();
	return 0;
}