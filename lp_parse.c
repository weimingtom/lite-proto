#include "lp_parse.h"
#include "lp_lex.h"

#define lp_tag(t, a)	( (((byte)t)<<3)|((byte)a) )
#define lp_at_token(p)	((lp_token*)lp_list_inx((p)->token_list, (p)->read_inx))
#define lp_check(p, t)	do{ \
							lp_token* lp_t = NULL; \
							if(		\
								(lp_t=lp_at_token(p)) || (lp_t->type==t_error || lp_t->type!=(t)) 	\
							  ) \
							{	lp_t=(lp_t)?(lp_t):( (--(p)->read_inx), lp_at_token(p) );	\
								print("parse[error line: %d] you are missing \"%s\" at \"%s\"!\n", (lp_t)->line, watchs(t), watchs(lp_t->type) );	\
								return LP_FAIL;	\
							}	\
						}while(0)

#define lp_watch(p, t)	do{lp_check(p, t); ((p)->read_inx)++;}while(0)
#define lp_get_token(p, t, d)	do{lp_check((p), (t)); (d)=lp_at_token(p); ((p)->read_inx)++;}while(0)

static int lp_parse_closure(lp_parse_env* lp_p, lp_list* lp_out, char* at_mes);
int lp_parse_message(lp_parse_env* lp_p, char* at_mes);

int get_parse_env(lp_parse_env* lp_p, lp_list* token_list)
{
	check_null(lp_p, LP_FAIL);
	check_null(token_list, LP_FAIL);
	lp_list_new(&lp_p->parse_out, sizeof(byte), NULL, NULL);
	lp_table_new(&lp_p->parse_table);
	lp_p->token_list = token_list;
	lp_p->read_inx = 0;
	return LP_TRUE;
}

int free_parse_env(lp_parse_env* lp_p)
{
	check_null(lp_p, LP_FAIL);
	
	lp_list_free(&lp_p->parse_out);
	lp_table_free(&lp_p->parse_table);
	
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

int lp_parse_push(lp_parse_env* lp_p, void* data, size_t len)
{
	size_t i=0;
	check_null(data, LP_FAIL);
	
	for(i=0; i<len; i++)
		lp_list_add(&lp_p->parse_out, ((byte*)data)+i);
	return LP_TRUE;
}


int lp_parse_message(lp_parse_env* lp_p, char* at_mes)
{
	unsigned int id = 0;
	lp_token* temp = NULL;
	lp_token* mes_name = NULL;
	lp_token* mes_id = NULL;
	lp_list temp_out = {0};
	lp_string mes = {0};

	
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
			
			if(lp_parse_closure(lp_p, &temp_out, (char*)mes.str.list_p) == LP_FAIL)
				goto C_END;
		}
		break;
	default:
		lp_watch(lp_p, t_lb);
		break;
	}
	
	id = (mes_id)?(atoi((char*)mes_id->name.str.list_p)):(0);
	
	lp_parse_push(lp_p, mes.str.list_p, mes.str.list_len+1);	// write message name
	lp_parse_push(lp_p, &id, sizeof(id));	// write message id
	lp_parse_push(lp_p, &temp_out.list_len, sizeof(size_t));	// write message body lens
	lp_parse_push(lp_p, temp_out.list_p, temp_out.list_len);	// write message body
	lp_table_add(&lp_p->parse_table, (char*)mes.str.list_p);

C_END:
	lp_string_free(&mes);
	lp_list_free(&temp_out);
	return LP_TRUE;
}

static int lp_parse_closure(lp_parse_env* lp_p, lp_list* lp_out, char* at_mes)
{
	byte tag = 0;
	lp_token* temp = NULL;
	lp_token* ide = NULL;
	check_null(lp_p, LP_FAIL);
	check_null(lp_out, LP_FAIL);
	check_null(at_mes, LP_FAIL);

	lp_watch(lp_p, t_lb);
	
	temp = lp_at_token(lp_p);
	if(temp == NULL)
		lp_watch(lp_p, t_rb);
	
	for( ; (temp && (temp->type!=t_rb)); temp=lp_at_token(lp_p) )
	{
		switch(temp->type)
		{
		case t_Kint32:
		case t_Kint64:
		case t_Kfloat32:
		case t_Kfloat64:
		case t_kstring:
		case t_ide:
			{
				lp_token* tt = NULL;
				lp_watch(lp_p, temp->type);
				tt= lp_at_token(lp_p);
				if(tt == NULL)
					lp_watch(lp_p, t_rb);
				if(tt->type == t_ll)			// int32[] a; 
				{	
					lp_watch(lp_p, t_rl);
					tag = lp_tag(temp->type, e_rep);
					lp_get_token(lp_p, t_ide, ide);
					lp_watch(lp_p, t_end);
				}
				else if(tt->type == t_ide)
				{
					ide = tt;
					tag = lp_tag(temp->type, e_reg);
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
					for(i=0; i<sizeof(tag); i++)
						lp_list_add(lp_out, &tag);
					if(temp->type == t_ide)		// push "info[]"
					{
						lp_string mes = lp_string_new(at_mes);
						lp_string_cat(&mes, '.');
						lp_string_cats(&mes, (char*)temp->name.str.list_p);
						if( lp_table_look(&lp_p->parse_table, (char*)temp->name.str.list_p) == LP_FAIL && 
							lp_table_look(&lp_p->parse_table, (char*)mes.str.list_p) == LP_FAIL
						  )	 
						{
							lp_string_free(&mes);
							print("parse[error line: %d] not find message \"%s\" !\n", temp->line, (char*)temp->name.str.list_p);
							return LP_FAIL;
						}
						lp_string_free(&mes);
						for(i=0; i<temp->name.str.list_len; i++)
							lp_list_add(lp_out, temp->name.str.list_p+i);
						lp_list_add(lp_out, &a);
					}
					for(i=0; i<ide->name.str.list_len; i++)	// push io
						lp_list_add(lp_out, ide->name.str.list_p+i);
					lp_list_add(lp_out, &a);
				}
			}
			break;
		case t_Kmessage:
			check_fail(lp_parse_message(lp_p, at_mes), LP_FAIL);
			break;
		default:
			print("parse[error line: %d] at {} not allow!\n", temp->line);
		}
	}

	lp_watch(lp_p, t_rb);
	return LP_TRUE;
}
