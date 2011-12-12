#include "lp_lex.h"
#include "lp_conf.h"
#include "lp_table.h"


 static lp_key lp_sk[] = {
	{"message", t_Kmessage}, {"int32", t_Kint32}, {"int64", t_Kint64}, {"string", t_kstring},
	{"float32", t_Kfloat32}, {"float64", t_Kfloat64}, {NULL, t_null}
};

#define lp_add_token(l_p, token)	lp_list_add( &((l_p)->lex_list), (byte*)(&(token)) )
#define next_char(p)				( (p)&&((p)->sp)&&(*((p)->sp)) )?( *((char*)((p)->sp))++ ):(0)
#define char_type(p, c)				( (p)->char_enum[(byte)(c)] )

int get_lex_env(lp_lex_env* le)
{
	int i=0;

	// init key
	check_null(le, LP_FAIL);
	for(i=0; lp_sk[i].s_key; i++)
	{
		int inx = _BKDRHash(lp_sk[i].s_key, 16);
		if(le->lp_k[inx].s_key)
		{
			print("lp_key is not null! key=\"%s\" \n", lp_key[inx]);
			return LP_FAIL;
		}
		else
			le->lp_k[inx] = lp_sk[i];
	}
	memset(le->char_enum, 0, size_t(le->char_enum));
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
	unsigned int line = 0;
	char at_char = 0;
	check_null(env_p, LP_FAIL);
	check_null(buff, LP_FAIL);
	for(;at_char=next_char(buff);)
	{
		switch(char_type(env_p, at_char))
		{
		case l_char:
			break;
		case l_num:
			break;
		case l_ass:
			break;
		case l_lb:
		case l_ll:
			break;
		case l_rb:
		case l_rl:
			break;
		case l_text:
			while(next_char(buff) != '\n');
		case  l_n:
			line++;
			break;
		default:
			print("lex[error line: %ud] find can not lex char!\n", line);
			return LP_FAIL;
		}
	}

	return LP_TRUE;
}
