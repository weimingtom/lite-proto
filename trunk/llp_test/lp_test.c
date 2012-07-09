#include "llp.h"
#include <windows.h>

#pragma comment(lib, "../llp/sllp/Debug/sllp.lib")


#define LENSS 1
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
		llp_Wmes_int32(lpm, "aa", 123);
		llp_Wmes_int32(lpm, "bb", 456);
		llp_Wmes_string(lpm, "cc", "hi, I am string!");
		
		llp_in_message(llp_out_message(lpm), lpm2);
		print("%d %d %s\n", llp_Rmes_int32(lpm2, "aa", 0),
			llp_Rmes_int32(lpm2, "bb", 0),
			llp_Rmes_string(lpm2, "cc", 0));
		llp_message_clr(lpm);
		llp_message_clr(lpm2);
	}
	tt[1] = GetTickCount();
	print("end time: %ld\n all time[lens = %ld]: %ld\n", tt[1], LENSS, tt[1]-tt[0]);
	llp_message_free(lpm);
	llp_message_free(lpm2);
}

void ex_test(llp_env* env)
{
	llp_mes* lp = llp_message_new(env, "testA");
	llp_mes* lp3 = llp_message_new(env, "testA");
	llp_mes* lp2=NULL;
	slice* sl;

	llp_Wmes_int32(lp, "ta2", 1234);
	llp_Wmes_string(lp, "ta3", "testA");
	lp2 = llp_Wmes_message(lp, "ta1");
	llp_Wmes_int32(lp2, "tb1", 11);
	llp_Wmes_int32(lp2, "tb2", 22);
	llp_Wmes_string(lp2, "tb3", "testB");
	
	sl = llp_out_message(lp);
	
	llp_in_message(sl, lp3);
	lp2 = llp_Rmes_message(lp3, "ta1", 0);
	print("ta1.tb1=%d\nta1.tb2=%d\nta1.tb3=%s\n%ta2=%d\nta3=%s\n",
		llp_Rmes_int32(lp2, "tb1", 0),
		llp_Rmes_int32(lp2, "tb2", 0),
		llp_Rmes_string(lp2, "tb3", 0),
		llp_Rmes_int32(lp3, "ta2", 0),
		llp_Rmes_string(lp3, "ta3", 0)
		);

	llp_message_free(lp);
	llp_message_free(lp3);

}

int main(void)
{
	byte buff[1024]={0};
	slice sl;
	long lens;
	llp_env* env = NULL;
	FILE* fp = NULL;
	env = llp_new_env();
	
// 	fp = fopen("at.mes.lpb", "rb");
// 	lens=f_size(fp);
// 	fread(buff, 1, lens, fp);
// 	sl.b_sp = sl.sp = buff;
// 	sl.sp_size = lens;
// 
// 	llp_reg_mes(env, "at.mes.lpb");
// 	llp_reg_Smes(env, &sl);
// 	
// 	at_test(env);
	


	
	llp_reg_mes(env, "../testA.mes.lpb");
//	llp_reg_mes(env, "../testB.mes.lpb");
	ex_test(env);

	llp_free_env(env);
	print_mem();
	getchar();
	return 0;
}