#include "lp.h"
#include "lp_conf.h"
#include <stdlib.h>
#include "lp_lex.h"

lp_env* get_lp_env()
{
	lp_env* ret = (lp_env*)malloc(sizeof(*ret));
	memset(ret, 0, sizeof(*ret));

	check_fail(lp_lex_init(&ret->lex_envV), NULL);
	return ret;
}