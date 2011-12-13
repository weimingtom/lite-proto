#include <stdlib.h>
#include <stdio.h>

size_t mem = 0;

void* lp_malloc(size_t len)
{
	mem++;
	return malloc(len);
}

void lp_free(void* p)
{
	mem--;
	free(p);
}

void mem_print()
{
	printf("mem = %d\n", mem);
}
