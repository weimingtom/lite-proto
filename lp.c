#include "lp.h"
#include "lp_conf.h"
#include <stdlib.h>
#include <stdio.h>

lp_env* get_lp_env()
{
	lp_env* ret = (lp_env*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));

	check_fail(get_lex_env(&ret->lex_envV), NULL);
	check_fail(get_parse_env(&ret->parse_envV, &ret->lex_envV.lex_list), NULL);
	return ret;
}

void free_lp_env(lp_env* lp)
{
	if(lp==NULL)
		return;
	free_lex_env(&lp->lex_envV);
	free_parse_env(&lp->parse_envV);
	free(lp);
}

int read_file(char* file_name, slice* sp)
{
	FILE* fp = NULL;
	long fs = 0;
	
	check_null(file_name, (print("parse file not exist!\n"), LP_FAIL));
	fp = fopen(file_name, "r");
	fs = fsize(fp);
	check_null(fp, (print("read file: %s is error!\n", file_name), LP_FAIL));
	sp->sp_len = (size_t)fs + 2;
	sp->sp = (byte*)malloc(sp->sp_len);
	memset(sp->sp, 0, sp->sp_len);
	sp->b_sp = sp->sp;

	if(fread(sp->sp, sizeof(char), sp->sp_len, fp) <0)
		return LP_FAIL;
	return LP_TRUE;
}

int lp_main(int argc, char* args[])
{
	FILE* fp = NULL;
	slice sp = {0};
	lp_env* lp = get_lp_env();

	if(read_file("test.mes", &sp)==LP_FAIL)
		goto END;
	if(lp_lex(&lp->lex_envV, &sp)==LP_FAIL)
		goto END;
	if(lp_parse(&lp->parse_envV)==LP_FAIL)
	{
		print("parse error!\n");
		goto END;
	}
	
	lp_lex_print(&lp->lex_envV);

	fp = fopen("test.lpb", "w");
	if(fp)
	{
		fwrite(lp->parse_envV.parse_out.list_p, lp->parse_envV.parse_out.list_len, 1, fp);
		fclose(fp);
	}
END:
	if(sp.b_sp)
		free(sp.b_sp);

	free_lp_env(lp);
	print("\n\n----memory----\nmem = %d", mem);
	getchar();
}