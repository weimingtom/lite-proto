#ifndef _LP_CONF_H_
#define _LP_CONF_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum{
	LP_FAIL,
	LP_TRUE,
	LP_NIL
};

typedef unsigned char byte;
#define check_value(e, qe, r)	do{if((e)==(qe)) return (r);}while(0)	
#define check_fail(e, r)		check_value(e, LP_FAIL, r)
#define  check_null(e, r)		check_value(e, NULL, r)	
#define  print printf

#endif