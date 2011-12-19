#ifndef  _LIB_IO_H_
#define  _LIB_IO_H_
#include "lp_conf.h"

#define  sl_emp(sl)		( ((sl)->sp_len) - (((sl)->sp)-((sl)->b_sp)) )
#define check_sl(sl)	do{		\
							if( (sl)==NULL || (sl)->sp==NULL || ((unsigned int)((sl)->sp-(sl)->b_sp))>(sl)->sp_len ) \
							return LP_FAIL; \
							else if( ((unsigned int)((sl)->sp-(sl)->b_sp))==(sl)->sp_len )	\
							return LP_END;	\
						}while(0)

typedef enum _e_ot{
	o_num,
	o_str,
	o_mes
}e_ot;

int sl_Rstr(slice* sl, char** out);
int sl_Rlens(slice* sl, unsigned int* out);
int sl_Rbyte(slice* sl, byte* out);
int sl_Ruint(slice* sl, size_t* out);


#endif