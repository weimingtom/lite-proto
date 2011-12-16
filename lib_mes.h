#ifndef  _LIB_MES_H_
#define  _LIB_MES_H_
#include "lp_conf.h"
#include "lib_lp.h"
#include "lp_list.h"

typedef struct _llp_mes;
typedef union _filed_value{
	int*	int32_P;
	long*	int64_P;
	float*  float32_P;
	double* float64_P;
	char**  string_P;
	struct _llp_mes*  mes_P;
}filed_value;

typedef struct _llp_mes{
	t_def_mes*	d_mes;			// message body struct
	
	filed_value* filed_V;
	size_t		 filed_lens;
}llp_mes;

#endif