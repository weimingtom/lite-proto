
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
		print("map[%d]{hash=%d hash_full=%d key=%s next=%d}\n", i+1, l_map->table[i].hash % l_map->size, 
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
		"a",
		"b",
		"c"
	};
	srand(GetTickCount());
	for(i=0; i<3; i++)
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
//	dump_map( *((llp_map**)(((size_t)str_pool)+4)) );
	lib_stringpool_free(str_pool);
}



int main(void)
{
//	test_stringpool();
	int ret = 0;
	llp_env* env = llp_new_env();
	ret = llp_reg_mes(env, "farm.mes.lpb");

	llp_free_env(env);
	
//	test_map();
	print_mem(); 
	return 0;
}