#include "lp_lex.h"
#include "lp_conf.h"
#include "lp_table.h"


 static lp_key lp_sk[] = {
	{"message", t_Kmessage}, {"int32", t_Kint32}, {"int64", t_Kint64}, {"string", t_kstring},
	{"float32", t_Kfloat32}, {"float64", t_Kfloat64}, {NULL, t_error}
};

#define now_char(p)					( *((char*)p->sp) )
#define next_char(p)				( (p)&&((p)->sp)&&(*((p)->sp)) )?( *((char*)((p)->sp))++ ):(0)
#define char_type(p, c)				( (p)->char_enum[(byte)(c)] )

static int lp_lex_char(lp_lex_env* env_p, slice* buff);
static int lp_lex_number(lp_lex_env* env_p, slice* buff);

static lp_token lp_new_token(lp_lex_env* env_p, byte tt, lp_string name)
{
	lp_token lp_ret = {0};
	lp_ret.line = env_p->line;
	lp_ret.name = name;
	lp_ret.type = tt;

	return lp_ret;
}

static void lp_add_token(lp_lex_env* env_p, lp_token lp_t)
{
	lp_list_add( &((env_p)->lex_list), (byte*)(&(lp_t)) );
}

static byte lp_look_key(lp_lex_env* env_p, char* str)
{
	int inx = 0;
	check_null(env_p, t_error);
	check_null(str, t_error);
	inx = _BKDRHash(str, LEX_KEY_MAX);
	
	return (env_p->lp_k[inx].s_tt==t_error)?(t_error):(env_p->lp_k[inx].s_tt);
}

int get_lex_env(lp_lex_env* le)
{
	int i=0;

	// init key
	check_null(le, LP_FAIL);
	for(i=0; lp_sk[i].s_key; i++)
	{
		int inx = _BKDRHash(lp_sk[i].s_key, LEX_KEY_MAX);
		if(le->lp_k[inx].s_key)
		{
			print("lp_key is not null! key=\"%s\" \n", le->lp_k[inx].s_key);
			return LP_FAIL;
		}
		else
			le->lp_k[inx] = lp_sk[i];
	}
	memset(le->char_enum, 0, sizeof(le->char_enum));
	le->char_enum['_']= l_char;
	le->char_enum['{']= l_lb;
	le->char_enum['}']= l_rb;
	le->char_enum['=']= l_ass;
	le->char_enum['[']= l_ll;
	le->char_enum[']']= l_rl;
	le->char_enum['#']= l_text;
	for(i='a'; i<='z'; i++)
		le->char_enum[i] = l_char;
	for(i='A'; i<='Z'; i++)
		le->char_enum[i] = l_char;
	for(i='0'; i<='9'; i++)
		le->char_enum[i] = l_num;
	
	check_fail(lp_list_new(&le->lex_list, sizeof(lp_token)), LP_FAIL);
	return LP_TRUE;
}


int lp_lex(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	check_null(env_p, LP_FAIL);
	check_null(buff, LP_FAIL);
	for(;at_char=next_char(buff);)
	{
		switch(char_type(env_p, at_char))
		{
		case l_char:
			lp_lex_char(env_p, buff);
			break;
		case l_num:
			lp_lex_number(env_p, buff);
			break;
		case l_ass:
			lp_add_token(env_p, lp_new_token(env_p, t_ass, lp_string_new(NULL)));
			break;
		case l_lb:
			lp_add_token(env_p, lp_new_token(env_p, t_lb, lp_string_new(NULL)));
			break;
		case l_ll:
			lp_add_token(env_p, lp_new_token(env_p, t_ll, lp_string_new(NULL)));
			break;
		case l_rb:
			lp_add_token(env_p, lp_new_token(env_p, t_rb, lp_string_new(NULL)));
			break;
		case l_rl:
			lp_add_token(env_p, lp_new_token(env_p, t_rl, lp_string_new(NULL)));
			break;
		case l_text:
			while(next_char(buff) != '\n');
		case  l_n:
			(env_p->line)++;
			break;
		default:
			print("lex[error line: %ud] find can not lex char!\n", env_p->line);
			return LP_FAIL;
		}
	}

	return LP_TRUE;
}


static int lp_lex_char(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	byte t_t;
	lp_string name = lp_string_new("");
	for(;	(at_char=next_char(buff)) && 
			(char_type(env_p, at_char)==l_char || char_type(env_p, at_char)==l_num) ;
	   )
	{
		lp_string_cat(&name, at_char);
	}
	
	t_t = lp_look_key(env_p, (char*)name.str.list_p);
	lp_add_token(
					env_p, 
					lp_new_token(env_p, (byte)((t_t)?(t_t):(t_ide)), name)
				);

	return LP_TRUE;

}

static int lp_lex_number(lp_lex_env* env_p, slice* buff)
{
	char at_char = 0;
	lp_string name = lp_string_new("");
	for(;  (at_char=next_char(buff)) && (char_type(env_p, at_char)==l_num); )
		lp_string_cat(&name, at_char);

	lp_add_token(env_p, lp_new_token(env_p, l_num, name));
	return LP_TRUE;
}