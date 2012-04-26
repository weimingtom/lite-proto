#ifndef _LIB_LP_H_
#define _LIB_LP_H_
#include "lp_conf.h"
#include "lib_table.h"

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
	reg_mes,		// .mes file
	def_mes,		// message
	def_field		// filed at message body 
}e_lt;

struct _nl{
	char* name;
	struct _nl* next;
};

// regedit .mes file 
typedef struct _t_reg_mes{
	struct _nl* mNs;		// all message at .mes
	size_t mNs_count;		// all message count 
	
/*	byte* mes_p;			// .mes buff
	size_t mes_size;		// .mes size
*/
}t_reg_mes;


struct _t_def_mes;
typedef struct _t_Mfield{
	byte tag;					// tag
	struct _t_def_mes* tms;		// if type message 
}t_Mfield;

// def message body
typedef struct _t_def_mes{
	llp_uint32		message_id;			// message id
	llp_table		message_filed;		// filed table
	t_Mfield*		message_tfl;		// filed tag list
	llp_uint32		message_count;		// filed count
}t_def_mes;

// def field at message
typedef struct _t_def_field{
	 int f_id;			// id
}t_def_field;

typedef struct _lp_value{
	union{
		t_reg_mes	reg_mesV;
		t_def_mes	def_mesV;
		t_def_field def_fieldV;
	}value;
}lp_value;

typedef struct _llp_env{
	llp_table mes;
	llp_table dmes;

	llp_strT mesN;
}llp_env;

#define DEF_MES_LEN		64
#define DEF_DMES_LEN    128
 
#define  tag_type(t)	( ((byte)t)>>3 )
#define  tag_state(t)	( ((byte)(t)) & 0x07 ) 

#endif