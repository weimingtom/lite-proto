#include "llp.h"
#include <windows.h>

#pragma comment(lib, "..\\llp\\sllp___Win32_Release\\sllp.lib")


#define LENSS 1000000
void at_test(llp_env* env)
{
	int ret = 0;
	int j=0;
	unsigned int i=0;
	unsigned long tt[2] = {0};
	llp_mes* lpm = NULL;
	llp_mes* lpm2 =NULL;
	
	lpm = llp_message_new(env, "at");
	lpm2 = llp_message_new(env, "at");

	tt[0] = GetTickCount();
	print("begin time: %ld\n", tt[0]);
	for(j=0; j<LENSS; j++)
	{
		ret = llp_Wmes_int32(lpm, "aa", 123);
		ret = llp_Wmes_int32(lpm, "bb", 456);
	//	for(i=0; i<10; i++)
			ret = llp_Wmes_string(lpm, "cc", "test string!");
		
	/*	print("aa = %d\n", llp_Rmes_int32(lpm, "aa", 0));
		print("bb = %d\n", llp_Rmes_int32(lpm, "bb", 0));
		for(i=0; i<llp_Rmes_size(lpm, "cc"); i++)
			print("cc = %s\n", llp_Rmes_string(lpm, "cc", i));
	*/	llp_in_message(llp_out_message(lpm), lpm2);
		llp_message_clr(lpm);
		llp_message_clr(lpm2);
	}
	tt[1] = GetTickCount();
	print("end time: %ld\n all time[lens = %ld]: %ld\n", tt[1], LENSS, tt[1]-tt[0]);
	llp_message_free(lpm);
}

int main(void)
{
	llp_env* env = NULL;
	
	env = llp_new_env();
	
	llp_reg_mes(env, "test.mes.lpb");
	llp_reg_mes(env, "at.mes.lpb");
	
	at_test(env);

	llp_free_env(env);
	print("mem = %d\n", mem);
	return 0;
}