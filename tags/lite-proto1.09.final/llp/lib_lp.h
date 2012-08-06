#ifndef _LIB_LP_H_
#define _LIB_LP_H_
#include "lp_conf.h"
#include "lib_table.h"
#include "lib_stringpool.h"

// filed type
typedef enum _e_lpt{
	lpt_int32 = 2,
	lpt_int64 = 3,
	lpt_string = 4,
	lpt_float32 = 5,
	lpt_float64 = 6,
	lpt_stream = 16,
	lpt_message = 13
}e_lpt;

typedef enum _e_lpf{
	lpf_req,		// required
	lpf_rep			// repeated
}e_lpf;

typedef enum _e_lt{
	def_mes,		// message
	def_field		// filed at message body 
}e_lt;

struct _t_def_mes;
typedef struct _t_Mfield{
	byte tag;					// tag
	struct _t_def_mes* tms;		// if type message 
}t_Mfield;

// def message body
typedef struct _t_def_mes{
	llp_uint32		message_id;			// message id
	filed_map*		message_filed;		// filed table
	t_Mfield*		message_tfl;		// filed tag list
	llp_uint32		message_count;		// filed count
}t_def_mes;

// reference self 
typedef struct _rs_node{
	t_def_mes* p;
	struct _rs_node* next;	
}rs_node;

typedef struct _llp_env{		
	llp_map* dmes;			// message body

	string_pool* mesN;		// string pool
	rs_node* rs_p;			// reference self list
}llp_env;


// #define DEF_MES_LEN		64
// #define DEF_DMES_LEN    128
 
#define  tag_type(t)	( ((byte)t)>>3 )
#define  tag_state(t)	( ((byte)(t)) & 0x07 ) 

#endif
