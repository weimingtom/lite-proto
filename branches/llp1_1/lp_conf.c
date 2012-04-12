#include <stdlib.h>
#include <stdio.h>

static size_t mem_lens = 0;

// 获取文件字节大小
long fsize( FILE *fp)
{
    long int save_pos;
    long size_of_file;
	
	if (fp == NULL)
		return 0;
	
    save_pos = ftell( fp );			// 保存当前文件指针地址
	
    fseek( fp, 0L, SEEK_END );		// 跳转到文件末尾
    size_of_file = ftell( fp );		// 获取文件开始末尾文件地址
    fseek( fp, save_pos, SEEK_SET ); // 恢复当前的文件地址
	
    return( size_of_file);
}

void* lp_malloc(size_t len)
{
	mem_lens++;
	
	return malloc(len);
}

void lp_free(void* p)
{
	mem_lens--;
	free(p);
}

void print_mem()
{
	int i=0;
	printf("mem = %d\n", mem_lens);
}