#ifndef  _LP_LEX_H
#define  _LP_LEX_H

typedef enum _e_c{
	l_null,
	l_char,
	l_num,
	l_lb,
	l_rb,
	l_ass,
	l_ll,
	l_rl,
	l_text
}e_c;

struct _lp_key{   
	char* s_key;
	byte s_tt;
}lp_key;

#define LEX_KEY_MAX 16
typedef struct _lp_lex_env{
	lp_key lp_k[LEX_KEY_MAX];
	byte char_enum[128];
	lp_list lex_list;
}lp_lex_env;


typedef enum _e_t{
	t_null,
	t_Kmessage,
	t_Kint32,
	t_Kint64,
	t_kstring,
	t_Kfloat32,
	t_Kfloat64,
	t_num,
	l_lb,
	l_rb,
	l_ass,
	l_ll,
	l_rl,
	l_n,
	t_ide
}e_t;

typedef struct _lp_token{
	unsigned int line;

}lp_token;

int get_lex_env(lp_lex_env* le);

#endif