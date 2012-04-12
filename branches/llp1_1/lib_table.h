#ifndef  _LIB_TABLE_H_
#define  _LIB_TABLE_H_
#include "lp_conf.h"


typedef struct _filed_slot {
	char* filed_name;
	int id;
	size_t next;
}filed_slot;

typedef struct _filed_map{
	size_t size;
	filed_slot* slot;
}filed_map;


typedef struct _llp_slot{
	size_t hash;
	char* key;
	void* vp;
	size_t next;
}llp_slot;

typedef struct _llp_map {
	size_t  size;
	size_t	cap;
	llp_slot* table;
}llp_map;

typedef struct _llp_kv {
	char* key;
	void* vp;
}llp_kv;

struct _t_def_mes;
llp_map* lib_map_new();
void lib_map_free(llp_map* l_map);
int lib_map_add(llp_map* l_map, llp_kv* kv);
void** lib_map_find(llp_map* l_map, const char* key);

struct _t_def_mes* lib_Mmap_add(llp_map* l_map, char* message_name);
struct _t_def_mes* lib_Mmap_find(llp_map* l_map, char* message_name);
void lib_Mmap_free(llp_map* l_map);

filed_map* lib_Fmap_new(size_t size);
void lib_Fmap_free(filed_map* f_map);
int lib_Fmap_add(filed_map* f_map,  char* filed_name, int id);
int* lib_Fmap_find(filed_map* f_map, char* filed_name);



/*

int lib_table_new(llp_table* lt, size_t size, byte type);
struct _lp_value* lib_table_add(llp_table* lt, char* name);
struct _lp_value* lib_table_look(llp_table* lt, char* name);
int lib_table_del(llp_table* lt, char* name);
int lib_table_free(llp_table* lt);
*/

#endif