#include "lib_table.h"
#include "lib_lp.h"
#include "lp_conf.h"

static void lib_tableOne_free(llp_table* lt, llp_tableO* lto);
int lib_table_free(llp_table* lt);


unsigned int _L_BKDRHash(char *str, int len)
{
	unsigned int seed = 131; 
	unsigned int hash = 0;
	
	while (*str)
		hash = hash * seed + (*str++);
	
	return (hash & 0x7FFFFFFF)%(len);
}


int lib_table_new(llp_table* lt, size_t size, byte type)
{
	check_null(lt, LP_FAIL);
	if(size==0)
		return LP_FAIL;
	
	lt->lens = size;
	lt->type = type;
	lt->table_p = (llp_tableO**)malloc(sizeof(llp_tableO*)*size);
	check_null(lt->table_p, LP_FAIL);
	memset((void*)(lt->table_p), 0, sizeof(llp_tableO*)*size);
	
	return LP_TRUE;
}

lp_value* lib_table_add(llp_table* lt, char* name)
{
	llp_tableO* np = NULL;
	int inx = 0;
	check_null(lt, NULL);
	check_null(name, NULL);
	
	inx = _L_BKDRHash(name, lt->lens);
	np = lt->table_p[inx];
	while(np)
	{
		if(strcmp(np->name, name)==0)
			return NULL;
		np = np->next;
	}

	np = (llp_tableO*)malloc(sizeof(llp_tableO));
	memset(np, 0, sizeof(llp_tableO));
	
	np->value = (lp_value*)malloc(sizeof(lp_value));
	memset(np->value, 0, sizeof(lp_value));
	
	np->name = name;
	np->next = lt->table_p[inx];
	lt->table_p[inx]=np;
	return np->value;
}

lp_value* lib_table_look(llp_table* lt, char* name)
{
	llp_tableO* np = NULL;
	int inx =0;
	check_null(lt, NULL);
	check_null(name, NULL);

	inx = _L_BKDRHash(name, lt->lens);
	np = lt->table_p[inx];
	while(np)
	{
		if(strcmp(np->name, name)==0)
			return np->value;
		np = np->next;
	}

	return NULL;
}

int lib_table_del(llp_table* lt, char* name)
{
	llp_tableO* b_np = NULL;
	llp_tableO* np = NULL;
	int inx = 0;
	check_null(lt, LP_FAIL);
	check_null(name, LP_FAIL);
	
	inx = _L_BKDRHash(name, lt->lens);
	np = lt->table_p[inx];
	
	while(np)
	{
		if(strcmp(np->name, name)==0)
		{
			llp_tableO* npt = np->next;
			lib_tableOne_free(lt, np);
			if(np == lt->table_p[inx])
				lt->table_p[inx] = npt;
			else
				b_np->next = npt;
			return LP_TRUE;
		}
		b_np = np;
		np = np->next;
	}
	
	return LP_FAIL;
}

static int lib_value_free(llp_table* lt, lp_value* lpv)
{	
	switch(lt->type)
	{
	case reg_mes:
		{
			struct _nl* p = lpv->value.reg_mesV.mNs;
			while(p)
			{
				struct _nl* np = p->next;
				free(p);
				p=np;
			}
		}
		break;
	case def_mes:
		{
			lib_table_free(&lpv->value.def_mesV.message_filed);
			free(lpv->value.def_mesV.message_tfl);
		}
		break;
	case def_field:
		break;
	default: 
		return LP_FAIL;
	}

	free(lpv);
	return LP_TRUE;
}

static void lib_tableOne_free(llp_table* lt, llp_tableO* lto)
{	
	lib_value_free(lt, lto->value);
	free(lto);
}

int lib_table_free(llp_table* lt)
{
	size_t i=0;
	check_null(lt, LP_FAIL);
	
	for(i=0; i<lt->lens; i++)
	{
		if(lt->table_p[i])
		{
			llp_tableO* lto = lt->table_p[i];
			llp_tableO* lto_next = NULL;
			while(lto)
			{
				lto_next = lto->next;
				lib_tableOne_free(lt, lto);
				lto = lto_next;
			}
			lt->table_p[i] = NULL;
		}
	}

	free((void*)lt->table_p);
	return LP_TRUE;
}


char* malloc_string(char* str)
{
	int len = 0;
	char* ret = NULL;
	check_null(str, NULL);
	len = strlen(str);
	ret = (char*)malloc(len+1);
	memcpy(ret, str, len);
	ret[len] = 0;

	return ret;
 }

char* lib_Stable_add(llp_strT* ls, char* name)
{
	llp_strTO* lsto = NULL;
	llp_strTO* next_lsto = NULL;
	int inx =0;
	
	check_null(ls, NULL);
	check_null(name, NULL);
	inx = _L_BKDRHash(name, DEF_STR_LEN);
	lsto = ls->lst[inx];
	while(lsto)
	{
		next_lsto = lsto->next;
		if(strcmp(lsto->str, name)==0)
			return lsto->str;
		lsto = next_lsto;
	}

	lsto = (llp_strTO*)malloc(sizeof(llp_strTO));
	memset(lsto, 0, sizeof(llp_strTO));
	lsto->str = malloc_string(name);
	
	lsto->next = ls->lst[inx];
	ls->lst[inx] = lsto;
	return lsto->str;
}

void lib_Stable_free(llp_strT* ls)
{
	int i=0;
	if(ls==NULL)
		return;

	for(i=0; i<DEF_STR_LEN; i++)
	{
		if(ls->lst[i])
		{
			llp_strTO* p = ls->lst[i];
			llp_strTO*np = NULL;
			while(p)
			{
				np = p->next;
				free(p->str);
				free(p);
				p=np;
			}
			ls->lst[i]=NULL;
		}
	}
}