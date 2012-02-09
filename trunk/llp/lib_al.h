#ifndef  _LIB_AL_H_
#define	 _LIB_AL_H_
#include "lp_conf.h"

struct _llp_mes;
typedef union _llp_value{
	char*			 lp_str;
	llp_int32		 lp_int32;
	llp_int64		 lp_int64;
	llp_float32		 lp_float32;
	llp_float64		 lp_float64;
	slice*			 lp_stream;
    struct _llp_mes* lp_mes;
}llp_value;


#define EXT_AL_LENS		32
typedef struct _llp_array{
	size_t lens;

	llp_value* vp;
	llp_value vt[1];	// defoult value;
}llp_array;


typedef  void (*al_free)(llp_value*);
int lib_array_new(llp_array* al_out);
int lib_array_add(llp_array* al, llp_value* llp_v);
llp_value* lib_array_inx(llp_array* al, unsigned int inx);
void lib_array_clr(llp_array* al, al_free free_func);
int lib_array_free(llp_array* al, al_free free_func);

#endif