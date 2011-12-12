#include "lp_table.h"
#include "lp_conf.h"
#include <memory.h>


#define  LP_LIST_DEF_SIZE		128

unsigned int _BKDRHash(char *str, int len)
{
	unsigned int seed = 131; 
	unsigned int hash = 0;
	
	while (*str)
		hash = hash * seed + (*str++);
	
	return (hash & 0x7FFFFFFF)%(len);
}


int lp_list_new(lp_list* lp_l, size_t one_size)
{
	check_null(lp_l, LP_FAIL);
	lp_l->one_size = one_size;
	lp_l->list_len = 0;
	lp_l->list_p = (byte*)malloc(one_size*LP_LIST_DEF_SIZE);
	memset(lp_l->list_p, 0, one_size*LP_LIST_DEF_SIZE);
	lp_l->list_size = LP_LIST_DEF_SIZE;

	return LP_TRUE;
}

int lp_list_relloc(lp_list* lp_l)
{
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_FAIL);

	if(lp_l->list_len < lp_l->list_size)
		return LP_NIL;

	check_null(realloc(lp_l->list_p, (lp_l->list_size+=LP_LIST_DEF_SIZE)*lp_l->one_size), LP_FAIL);
	return LP_TRUE;
}

int lp_list_add(lp_list* lp_l, byte* data_p)
{
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_FAIL);
	check_null(data_p, LP_FAIL);

	check_fail(lp_list_relloc(lp_l), LP_FAIL);
	memcpy((lp_l->list_p + lp_l->one_size*lp_l->list_len), data_p, lp_l->one_size);
	(lp_l->list_len)++;

	return LP_TRUE;
}


int lp_list_free(lp_list* lp_l)
{
	check_null(lp_l, LP_FAIL);
	
	free(lp_l->list_p);
	memset(lp_l, 0, sizeof(lp_list));
	return LP_TRUE;
}