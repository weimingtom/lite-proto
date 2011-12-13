#ifndef  _LP_TABLE_H_
#define  _LP_TABLE_H_
#include "lp_conf.h"

typedef int (*list_Fnew)(void*);
typedef int (*list_Ffree)(void*);
typedef struct _lp_list{
	size_t list_len;
	size_t list_size;
	byte* list_p;
	
	list_Fnew  new_func;
	list_Ffree free_func;
	size_t one_size;
}lp_list;

typedef struct _slice{
	byte* sp;
	byte* b_sp;
	size_t sp_len;
}slice;


typedef struct _lp_string{
	lp_list str;
}lp_string;

typedef struct _lp_table_one{
	byte	tag;			// _ _ _ _ _is type  _ _ _ is or ont al
	lp_string name;
	struct _lp_table_one* next;
}lp_table_one;

#define  TABLE_SIZE		512
typedef struct _lp_table{
	lp_table_one* table_list[TABLE_SIZE];
	size_t table_len;
}lp_table;


unsigned int _BKDRHash(char *str, int len);
int lp_table_new(lp_table* lp_t);
int lp_table_add(lp_table* lp_t, char* name);
int lp_table_look(lp_table* lp_t, char* name);
int lp_table_free(lp_table* lp_t);

int lp_list_new(lp_list* lp_l, size_t one_size, list_Fnew new_func, list_Ffree free_func);
int lp_list_add(lp_list* lp_l, byte* data_p);
void* lp_list_inx(lp_list* lp_l, size_t inx);
int lp_list_free(lp_list* lp_l);

lp_string lp_string_new(char* str);
lp_string* lp_string_cat(lp_string* lp_s, char at_char);
lp_string* lp_string_cats(lp_string* lp_s, char* str);
void lp_string_free(lp_string* lp_s);

#endif