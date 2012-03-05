#ifndef  _LIB_TABLE_H_
#define  _LIB_TABLE_H_
#include "lp_conf.h"


struct _lp_value;
typedef struct _llp_tableO{
	char* name;
	struct _lp_value* value;
	
	struct _llp_tableO* next;
}llp_tableO;


typedef struct _llp_table{
	llp_tableO**  table_p;
	size_t		  lens;
	byte		  type;
}llp_table;

typedef struct _llp_strTO{
	char* str;
	struct _llp_strTO* next;
}llp_strTO;

#define DEF_STR_LEN		128
typedef struct _llp_strT{
	llp_strTO* lst[DEF_STR_LEN];	
}llp_strT;

int lib_table_new(llp_table* lt, size_t size, byte type);
struct _lp_value* lib_table_add(llp_table* lt, char* name);
struct _lp_value* lib_table_look(llp_table* lt, char* name);
int lib_table_del(llp_table* lt, char* name);
int lib_table_free(llp_table* lt);


char* lib_Stable_add(llp_strT* ls, char* name);
void lib_Stable_free(llp_strT* ls);

#endif