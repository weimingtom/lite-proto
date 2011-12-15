#ifndef	_LP_LIST_H_
#define _LP_LIST_H_
#include "lp_conf.h"

#define  LP_LIST_DEF_SIZE		128

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

int lp_list_new(lp_list* lp_l, size_t one_size, list_Fnew new_func, list_Ffree free_func);
int lp_list_add(lp_list* lp_l, byte* data_p);
void* lp_list_inx(lp_list* lp_l, size_t inx);
int lp_list_free(lp_list* lp_l);

#endif
