#ifndef _LIB_LP_H_
#define _LIB_LP_H_
#include "lp_conf.h"
#include "lib_table.h"
#include "lp_list.h"

// filed type
typedef enum _e_lpt{
	lpt_int32 = 2,
	lpt_int64 = 3,
	lpt_string = 4,
	lpt_float32 = 5,
	lpt_float64 = 6,
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

	byte* mes_p;			// .mes buff
	size_t mes_size;		// .mes size
	
	size_t mNs_count;		// all message count 
}t_reg_mes;

// def message body
typedef struct _t_def_mes{
	unsigned int	message_id;
	llp_table		message_filed;
}t_def_mes;

// def field at message
typedef struct _t_def_field{
	int f_id;			// id
	byte tag;			// tag
	char* f_name;		// filed name
	t_def_mes* mes;		// if type is message ,not is NULL
}t_def_field;

typedef struct _lp_value{
	union{
		t_reg_mes	reg_mesV;
		t_def_mes	def_mesV;
		t_def_field def_fieldV;
	}value;
	
	e_lt type;
}lp_value;

typedef struct _llp_env{
	llp_table mes;
	llp_table dmes;

	llp_strT mesN;
}llp_env;

#define DEF_MES_LEN		64
#define DEF_DMES_LEN    128
 
static int get_llp_env(llp_env* env_p);
static void free_llp_env(llp_env* p);

#endif