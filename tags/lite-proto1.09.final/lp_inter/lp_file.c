#include "lp.h"
#include "lp_conf.h"
#include "lp_file.h"
#include <IO.h>

int read_file(char* file_name, slice* sp)
{
	FILE* fp = NULL;
	long fs = 0;
	
	check_null(file_name, (print("parse file not exist!\n"), LP_FAIL));
	fp = fopen(file_name, "rb");
	fs = f_size(fp);
	check_null(fp, (print("read file: %s is error!\n", file_name), LP_FAIL));
	sp->sp_size = (size_t)fs + 2;
	sp->sp = (byte*)malloc(sp->sp_size);
	memset(sp->sp, 0, sp->sp_size);
	sp->b_sp = sp->sp;
	
	if(fread(sp->sp, sizeof(char), sp->sp_size, fp) <0)
		return LP_FAIL;
	return LP_TRUE;
}

int lp_path(lp_env* lp, char* path)
{
	struct _finddata_t  filefind = {0};
	lp_string t_path = lp_string_new(path);
	int handle = 0;

	check_null(path, (print("inter fail<path: null>!\n"), LP_FAIL));
	lp_string_cats(&t_path, "\\*.* ");
	if((handle=_findfirst((char*)t_path.str.list_p, &filefind))==-1)
	{
		print("inter fail<path: %s is not path>!\n", path);
		lp_string_free(&t_path);
		return LP_FAIL;
	}
	
	lp_string_free(&t_path);
	
	while(!(_findnext(handle,&filefind)))
	{
		if(filefind.name[0]=='.')
			continue;		// 过滤隐藏文件

		// 获取文件
		if(_A_ARCH==filefind.attrib && strcmp(filefind.name+(strlen(filefind.name)-4), ".mes")==0)
		{
			lp_string t_name = lp_string_new(path);
			lp_string_cats(&t_name, "\\");
			lp_string_cats(&t_name, filefind.name);
			lp_inter(lp, (char*)t_name.str.list_p);
			lp_string_free(&t_name);
		}
	}
	return LP_TRUE;
}