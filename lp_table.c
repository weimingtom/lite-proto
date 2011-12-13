#include "lp_table.h"
#include "lp_conf.h"
#include <memory.h>


#define  LP_LIST_DEF_SIZE		128
#define  lp_hash(str)	_BKDRHash(str, TABLE_SIZE)
int lp_table_one_free(lp_table_one* lt_o);


unsigned int _BKDRHash(char *str, int len)
{
	unsigned int seed = 131; 
	unsigned int hash = 0;
	
	while (*str)
		hash = hash * seed + (*str++);
	
	return (hash & 0x7FFFFFFF)%(len);
}

int lp_table_new(lp_table* lp_t)
{
	check_null(lp_t, LP_FAIL);
	lp_t->table_len = 0;
	memset(lp_t->table_list, 0, sizeof(lp_table_one*));
	
	return LP_TRUE;
}

int lp_table_add(lp_table* lp_t, char* name)
{
	lp_table_one* lt_o = NULL;
	lp_table_one* head = NULL;
	int inx = 0;
	check_null(lp_t, LP_FAIL);
	check_null(name, LP_FAIL);
	
	inx = lp_hash(name);
	head = lp_t->table_list[inx];
	if(head==NULL)
	{
		lp_t->table_list[inx] = (lp_table_one*)malloc(sizeof(lp_table_one));
		memset(lp_t->table_list[inx], 0, sizeof(lp_table_one));
		lp_t->table_list[inx]->name = lp_string_new(name);
	}
	else
	{
		lp_table_one* back = NULL;
		while(head)
		{
			if(strcmp((char*)head->name.str.list_p, name)==0)
			{
				return LP_EXIST;
			}
			back = head;
			head = head->next;
		}
		head->next = (lp_table_one*)malloc(sizeof(lp_table_one));
		memset(head->next, 0, sizeof(lp_table_one));
		head->next->name = lp_string_new(name);
	}
	return LP_TRUE;
}

int lp_table_look(lp_table* lp_t, char* name)
{
	lp_table_one* lt_o = NULL;
	int inx =0;
	check_null(lp_t, LP_FAIL);
	check_null(name, LP_FAIL);
	
	inx = lp_hash(name);
	lt_o = lp_t->table_list[inx];
	while(lt_o)
	{
		if(strcmp((char*)lt_o->name.str.list_p, name)==0)
			return LP_TRUE;
		lt_o = lt_o->next;
	}
	return LP_FAIL;
}

int lp_table_free(lp_table* lp_t)
{
	int i=0;
	check_null(lp_t, LP_FAIL);
	for(i=0; i<TABLE_SIZE; i++)
	{
		if(lp_t->table_list[i])
		{
			lp_table_one* lt_o = lp_t->table_list[i];
			while(lt_o)
			{
				lp_table_one* nt = lt_o->next;
				lp_table_one_free(lt_o);
				lt_o = nt;
			}
			lp_t->table_list[i] = NULL;
		}
	}
	return LP_TRUE;
}

int lp_table_one_free(lp_table_one* lt_o)
{
	check_null(lt_o, LP_FAIL);
	lp_string_free(&lt_o->name);
	free(lt_o);
	return LP_TRUE;
}


int lp_list_new(lp_list* lp_l, size_t one_size, list_Fnew new_func, list_Ffree free_func)
{
	check_null(lp_l, LP_FAIL);
	lp_l->one_size = one_size;
	lp_l->list_len = 0;
	lp_l->list_p = (byte*)malloc(one_size*LP_LIST_DEF_SIZE);
	memset(lp_l->list_p, 0, one_size*LP_LIST_DEF_SIZE);
	lp_l->list_size = LP_LIST_DEF_SIZE;
	lp_l->new_func = new_func;
	lp_l->free_func = free_func;

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
	if(lp_l->new_func)
		lp_l->new_func(data_p);
	memcpy((lp_l->list_p + lp_l->one_size*lp_l->list_len), data_p, lp_l->one_size);
	(lp_l->list_len)++;

	return LP_TRUE;
}

void* lp_list_inx(lp_list* lp_l, size_t inx)
{
	check_null(lp_l, NULL);
	check_null(lp_l->list_p, NULL);

	return (inx>=lp_l->list_len)?(NULL):( (void*)(&lp_l->list_p[lp_l->one_size*inx]) );
}


int lp_list_free(lp_list* lp_l)
{
	size_t i=0;
	check_null(lp_l, LP_FAIL);
	check_null(lp_l->list_p, LP_NIL);

	for(i=0; i<lp_l->list_len; i++)
	{
		if(lp_l->free_func)
			lp_l->free_func(lp_list_inx(lp_l, i));
	}
	free(lp_l->list_p);
	memset(lp_l, 0, sizeof(lp_list));
	return LP_TRUE;
}

lp_string lp_string_new(char* str)
{
	lp_string lp_s = {0};
	check_null(str, lp_s);
	lp_list_new(&lp_s.str, 1, NULL, NULL);
	while(*str)
	{
		lp_list_add(&lp_s.str, str);
		str++;
	}

	lp_list_add(&lp_s.str, str);
	lp_s.str.list_len--;
	return lp_s;
}

lp_string* lp_string_cats(lp_string* lp_s, char* str)
{
	size_t i=0;
	check_null(lp_s, NULL);
	check_null(str, NULL);
	for(i=0; i<strlen(str)+1; i++)
		lp_list_add(&lp_s->str, str+i);
	lp_s->str.list_len--;
	return lp_s;
}

lp_string* lp_string_cat(lp_string* lp_s, char at_char)
{
	char a = 0;
	check_null(lp_s, NULL);
	
	lp_list_add(&lp_s->str, &at_char);
	lp_list_add(&lp_s->str, &a);
	lp_s->str.list_len--;
	return lp_s;
}

void lp_string_free(lp_string* lp_s)
{
	if(lp_s==NULL)
		return;

	lp_list_free(&lp_s->str);
}