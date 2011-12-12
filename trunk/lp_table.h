#ifndef  _LP_TABLE_H_
#define  _LP_TABLE_H_

typedef  union _Lvalue{
	char* str;
}Lvalue;  

typedef struct _lp_Mone{
	char*	name;
	Lvalue  value;
	struct _lp_Mone* next;
}lp_Mone;

typedef struct lp_map{
	lp_Mone** lp_p;
};

typedef struct _lp_list{
	size_t list_len;
	size_t list_size;
	byte* list_p;

	size_t one_size;
}lp_list;

typedef struct _slice{
	byte* sp;
	size_t sp_len;
}slice;

unsigned int _BKDRHash(char *str, int len);
int lp_list_new(lp_list* lp_l, size_t one_size);
int lp_list_add(lp_list* lp_l, byte* data_p);
int lp_list_free(lp_list* lp_l);

#endif