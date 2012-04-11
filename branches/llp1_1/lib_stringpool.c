#include "lp_conf.h"
#include "lib_stringpool.h"

//  read from pbc 

typedef struct _string_pool{
	char*  str;
	size_t len;
	struct _string_pool* next;
}string_pool;


// the str's lens at string pool
#define STR_PAGE_SIZE		 256

string_pool* lib_stringpool_new()
{
	string_pool* ret = (string_pool*)malloc(sizeof(string_pool) + STR_PAGE_SIZE);
	ret->str = (char*)(ret+1);
	ret->len = 0;
	ret->next = NULL;
	
	return ret;
}


void lib_stringpool_free(string_pool* sp)
{
	while(sp)
	{
		string_pool* next = sp->next;
		free(sp);
		sp = next;
	}
}


char* lib_stringpool_add(string_pool* sp, const char* str)
{
	string_pool* ret = NULL;
	size_t len=0;
	check_null(sp, NULL);
	check_null(str, NULL);
	check_fail(str[0], NULL);

	len=strlen(str)+1;
	
	if(len <= STR_PAGE_SIZE - sp->len)
	{
		char* ret = sp->str + sp->len;
		memcpy(ret, str, len);
		sp->len +=len;
		return ret;
	}

	if(len > STR_PAGE_SIZE)
	{
		ret = (string_pool*)malloc(sizeof(string_pool) + len);
		ret->len = len;
		ret->next = sp->next;
		sp->next = ret;
		memcpy(ret->str, str, len);

		return ret->str;
	}
	

	ret = (string_pool*)malloc(sizeof(string_pool) + STR_PAGE_SIZE);
	ret->str = sp->str;
	ret->len = sp->len;
	ret->next = sp->next;

	sp->next = ret;
	sp->str = (char*)(ret+1);
	sp->len = len;
	memcpy(sp->str, str, len);
	return sp->str;
}