#include "lp_parse.h"
#include "lp_lex.h"

#define lp_tag(t, a)	( (((byte)t)<<3)|((byte)a) )
#define lp_at_token(p)	((lp_token*)lp_list_inx((p)->token_list, (p)->read_inx))
#define lp_check(p, t)	do{ \
							lp_token* lp_t = NULL; \
							if(		\
								(lp_t=lp_at_token(p))==NULL || (lp_t->type==t_error || lp_t->type!=(t)) 	\
							  ) \
							{\
							print("parse[error line: %d] you are missing \"%s\" at \"%s\"!\n", (p)->line, watchs(t), (lp_t==NULL)?("text end"):( (lp_t->name.str.list_p)?((char*)lp_t->name.str.list_p):watchs(lp_t->type) ) );	\
								return LP_FAIL;	\
							}	\
							else	\
								(p)->line = lp_t->line;	\
						}while(0)

#define lp_watch(p, t)	do{lp_check(p, t); ((p)->read_inx)++;}while(0)
#define lp_get_token(p, t, d)	do{lp_check((p), (t)); (d)=lp_at_token(p); ((p)->read_inx)++;}while(0)

static int lp_parse_closure(lp_parse_env* lp_p, lp_list* lp_out, llp_uint32* out_count, lp_table* ide_table, char* at_mes);
static int lp_parse_message(lp_parse_env* lp_p, char* at_mes);
static int lp_parse_defM(lp_parse_env* lp_p, char* at_mes, lp_string* out_name);

int get_parse_env(lp_parse_env* lp_p, lp_list* token_list)
{
	check_null(lp_p, LP_FAIL);
	check_null(token_list, LP_FAIL);
	lp_list_new(&lp_p->parse_out, sizeof(byte), NULL, NULL);
	lp_table_new(&lp_p->parse_table);
	
	lp_p->mes_name = lp_string_new("");
	lp_p->token_list = token_list;
	lp_p->read_inx = 0;
	lp_p->line = 0;
	return LP_TRUE;
}

int free_parse_env(lp_parse_env* lp_p)
{
	check_null(lp_p, LP_FAIL);
	
	lp_list_free(&lp_p->parse_out);
	lp_table_free(&lp_p->parse_table);
	lp_string_free(&lp_p->mes_name);

	return LP_TRUE;
}

int lp_parse(lp_parse_env* lp_p)
{
	int i=0;
	check_null(lp_p, LP_FAIL);
	check_null(lp_p->token_list, LP_FAIL);
	
	while(lp_p->read_inx < lp_p->token_list->list_len)
	{
		lp_token* lp_t = lp_at_token(lp_p);
		assert(lp_t);
		switch(lp_t->type)
		{
		case t_Kmessage:
			check_fail(lp_parse_message(lp_p, NULL), LP_FAIL);
			break;
		default:
			print("parse[error line: %d] the error token!\n", lp_t->line);
			return LP_FAIL;
		}
	}

	return LP_TRUE;
}

void lp_parse_push_uint32(lp_parse_env* lp_p, llp_uint32 data)
{
	byte temp = 0;
	temp = (byte)(data & 0x000000FF);
	lp_list_add(&lp_p->parse_out, &temp);

	temp = (byte)(data & ((0x0000FF00)>>8));
	lp_list_add(&lp_p->parse_out, &temp);

	temp = (byte)(data & ((0x00FF0000)>>16));
	lp_list_add(&lp_p->parse_out, &temp);

	temp = (byte)(data & ((0xFF000000)>>24));
	lp_list_add(&lp_p->parse_out, &temp);
}

int lp_parse_push(lp_parse_env* lp_p, void* data, unsigned int len)
{
	unsigned int i=0;
	check_null(data, LP_FAIL);
	
	for(i=0; i<len; i++)
		lp_list_add(&lp_p->parse_out, ((byte*)data)+i);
	return LP_TRUE;
}


static int lp_parse_message(lp_parse_env* lp_p, char* at_mes)
{
	llp_uint32 out_count = 0;
	int a_ret = LP_FAIL;
	int ret = LP_FAIL;
	llp_uint32 id = 0;
	lp_token* temp = NULL;
	lp_token* mes_name = NULL;
	lp_token* mes_id = NULL;
	lp_list temp_out = {0};
	lp_string mes = {0};
	lp_table ide_table = {0};
	
	check_null(lp_p, LP_FAIL);
	check_null(lp_p->token_list, LP_FAIL);

	lp_watch(lp_p, t_Kmessage);				// message
	lp_get_token(lp_p, t_ide, mes_name);	// test
	
	temp = lp_at_token(lp_p);		
	if(temp==NULL)
		lp_watch(lp_p, t_lb);
	
	switch(temp->type)					
	{
	case t_ass:								// if =
		lp_watch(lp_p, t_ass);
		lp_get_token(lp_p, t_num, mes_id);	// 1223
	case t_lb:
		{
			lp_list_new(&temp_out, 1, NULL, NULL);
			mes = lp_string_new("");
			if(at_mes)
			{
				lp_string_cats(&mes, at_mes);
				lp_string_cat(&mes, '.');
			}
			lp_string_cats(&mes, (char*)mes_name->name.str.list_p);
			if((a_ret=lp_table_add(&lp_p->parse_table, (char*)mes.str.list_p)) == LP_EXIST)
			{
				print("parse[error line: %d]the new message \"%s\" is exist!\n", lp_p->line, mes.str.list_p);
				goto C_END;
			}
			else if(a_ret==LP_FAIL)
				goto C_END;
			lp_table_new(&ide_table);
			out_count = 0;
			if(lp_parse_closure(lp_p, &temp_out, &out_count, &ide_table, (char*)mes.str.list_p) == LP_FAIL)
				goto C_END;
		}
		break;
	default:
		lp_watch(lp_p, t_lb);
		break;
	}
	
	id = (llp_uint32)((mes_id)?(atoi((char*)mes_id->name.str.list_p)):(0));

	lp_parse_push(lp_p, mes.str.list_p, mes.str.list_len+1);	// write message name
	lp_parse_push_uint32(lp_p, id);								// write message id
	lp_parse_push_uint32(lp_p, out_count);						// write message count
	lp_parse_push(lp_p, temp_out.list_p, temp_out.list_len);	// write message body
	
	ret = LP_TRUE;

C_END:
	lp_table_free(&ide_table);
	lp_string_free(&mes);
	lp_list_free(&temp_out);
	return ret;
}

static int lp_parse_closure(lp_parse_env* lp_p, lp_list* lp_out, llp_uint32* out_count, lp_table* ide_table, char* at_mes)
{
	size_t back_out_lens = 0;
	byte tag = 0;
	lp_token* temp = NULL;
	lp_token* ide = NULL;
	check_null(lp_p, LP_FAIL);
	check_null(lp_out, LP_FAIL);
	check_null(at_mes, LP_FAIL);
	check_null(ide_table, LP_FAIL);
	check_null(out_count, LP_FAIL);
	back_out_lens = lp_out->list_len;

	lp_watch(lp_p, t_lb);
	
	temp = lp_at_token(lp_p);
	if(temp == NULL)
		lp_watch(lp_p, t_rb);
	
	for( ;temp=lp_at_token(lp_p); )
	{
		switch(temp->type)
		{
		case t_Kint32:
		case t_Kint64:
		case t_Kfloat32:
		case t_Kfloat64:
		case t_kstring:
		case t_Kstream:
		case t_ide:
			{
				lp_token* tt = NULL;
				if(temp->type != t_ide)
					lp_watch(lp_p, temp->type);
				else
				{
					lp_string_clear(&lp_p->mes_name);
					check_fail(lp_parse_defM(lp_p, at_mes, &lp_p->mes_name), LP_FAIL);
				}
				tt= lp_at_token(lp_p);
				if(tt == NULL)
					lp_watch(lp_p, t_rb);
				if(tt->type == t_ll)			// int32[] a; 
				{	
					lp_watch(lp_p, t_ll);
					lp_watch(lp_p, t_rl);
					tag = lp_tag(temp->type, e_rep);
					lp_get_token(lp_p, t_ide, ide);
					lp_watch(lp_p, t_end);
				}
				else if(tt->type == t_ide)
				{
					ide = tt;
					tag = lp_tag(temp->type, e_req);
					lp_watch(lp_p, t_ide);
					lp_watch(lp_p, t_end);		// ;
				}
				else
				{
					print("parse[error line: %d]not find ide!\n", tt->line);
					return LP_FAIL;
				}

				{
					char a=0;
					size_t i=0;
					int ret = 0;
					if( (ret=lp_table_add(ide_table, (char*)ide->name.str.list_p))==LP_EXIST)
					{
						print("parse[error line: %d] the ide \"%s\" is redef!\n", lp_p->line, (char*)ide->name.str.list_p);
						return LP_FAIL;
					}
					else if(ret == LP_FAIL)
					{
						print("Serious error[lin: %d]: add ide is error! \n", lp_p->line);
						return LP_FAIL;
					}
					for(i=0; i<sizeof(tag); i++)
						lp_list_add(lp_out, &tag);
					if(temp->type==t_ide)				// push message type name
					{
						for(i=0; i<lp_p->mes_name.str.list_len; i++)
							lp_list_add(lp_out, lp_p->mes_name.str.list_p+i);
						lp_list_add(lp_out, &a);
					}
					for(i=0; i<ide->name.str.list_len; i++)	// push  ide name
						lp_list_add(lp_out, ide->name.str.list_p+i);
					lp_list_add(lp_out, &a);
					(*out_count)++;
				}
			}
			break;
		case t_Kmessage:
			check_fail(lp_parse_message(lp_p, at_mes), LP_FAIL);
			break;
		case t_rb:
			lp_watch(lp_p, t_rb);
			if(back_out_lens >= lp_out->list_len)
			{
				print("parse[error line: %d] at message \"%s\" is empty body!\n", lp_p->line, at_mes);
				return LP_FAIL;
			}
			goto CLO_END;
		default:
			print("parse[error line: %d] at {} not allow!\n", temp->line);
			return LP_FAIL;
		}
	}
	
	print("parse[error line: %d] you are lose \"}\"!\n", lp_p->line);
	return LP_FAIL;
CLO_END:
	return LP_TRUE;
}

static int lp_parse_defM(lp_parse_env* lp_p, char* at_mes, lp_string* out_name)
{
	lp_token* o_clo = NULL;
	size_t i=0;
	int ret = LP_FAIL;
	lp_string name = {0};
	lp_string full_name = {0};
	lp_token* np = NULL;

	check_null(lp_p, LP_FAIL);
	check_null(out_name, LP_FAIL);
	lp_get_token(lp_p, t_ide, o_clo);
	
	at_mes = (at_mes)?(at_mes):("");
	full_name = lp_string_new(at_mes);
	name = lp_string_new((char*)o_clo->name.str.list_p);

	for(;;)
	{
		np = lp_at_token(lp_p);
		if(np==NULL)
			goto DEFM_ERROR_END;
		
		switch(np->type)
		{
		case t_clo:
			lp_watch(lp_p, t_clo);
			np = lp_at_token(lp_p);
			if(np==NULL || np->type!=t_ide)
				goto DEFM_ERROR_END;
			else
			{
				lp_string_cat(&name, '.');
				lp_string_cats(&name, (char*)np->name.str.list_p);
				lp_watch(lp_p, t_ide);
			}
			break;
		case t_ll:
		case t_ide:
			{
				lp_string_cat(&full_name, '.');
				lp_string_cats(&full_name, (char*)name.str.list_p);
				if(lp_table_look(&lp_p->parse_table, (char*)full_name.str.list_p)==LP_TRUE)		// search at local
				{
					ret = LP_TRUE, lp_string_cats(out_name, (char*)full_name.str.list_p);
					goto DEFM_END;
				}
				else if(lp_table_look(&lp_p->parse_table, (char*)name.str.list_p)==LP_TRUE)			// search at global
				{
					ret = LP_TRUE, lp_string_cats(out_name, (char*)name.str.list_p);
					goto DEFM_END;
				}
				else
				{
					print("parse[line line: %d] not find message body \"%s\" !\n", lp_p->line, (char*)name.str.list_p);
					goto DEFM_ERROR_END;
				}
			}
			break;
		default:
			goto DEFM_ERROR_END;
		}
	}

DEFM_ERROR_END:
	print("parse[error line: %d] the message def is error!\n", lp_p->line);
DEFM_END:
	lp_string_free(&name);
	lp_string_free(&full_name);
	return ret;
}