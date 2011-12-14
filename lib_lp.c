#include "lib_lp.h"
#include "lp_conf.h"
#include <stdio.h>

int main(void)
{
	FILE* fp = fopen("test.lpb", "r"); 
	long lens = fsize(fp);

	check_null(fp, LP_FAIL);
	return 0;
}