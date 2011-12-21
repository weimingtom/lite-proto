#include "llp.h"

#pragma comment(lib, "..\\llp\\Debug\\llp.lib")

#define LENS 40
void test2_p_mes(llp_mes* lpm)
{
	int i=0;
	print("test2 id= %d \n", llp_Rmes_int64(lpm, "id", 0));
	print("test2 name = %s \n", llp_Rmes_string(lpm, "name", 0));
	print("test2 rgb = %d \n", llp_Rmes_int32(lpm, "name", 0));
	for(i=0; i<LENS; i++)
		print("test2 al[%d] = %d\n", i, llp_Rmes_int32(lpm, "al", i));
	{
		llp_mes* nms = NULL;
		for(i=0; i<4; i++)
		{
			nms = llp_Wmes_message(lpm, "i_al");
			llp_Wmes_int32(nms, "al", i+10);
			llp_Wmes_int32(nms, "b", i+100);
		}
		for(i=0; i<4; i++)
		{
			nms = llp_Rmes_message(lpm, "i_al", i);
			print("info al[%d]= %d\n", i, llp_Rmes_int32(nms, "al", 0));
			print("info b[%d] = %d\n", i, llp_Rmes_int32(nms, "b", 0));
		}
	}
}

void test2(llp_env* env)
{
	int i=0;
	llp_mes* lpm = NULL;
	llp_mes* lpm2 = NULL;

	// test1 message 
	lpm = llp_message_new(env, "test1");		// create a message obj
	llp_Wmes_int32(lpm, "a", 1523698);
	llp_Wmes_int32(lpm, "b", 456);
	llp_Wmes_string(lpm, "name", "test_name~~");
	
	print("test1 a= %d \n", llp_Rmes_int32(lpm, "a", 0));
	print("test1 b = %d \n", llp_Rmes_int32(lpm, "b", 0));
	print("test1 name = %s \n", llp_Rmes_string(lpm, "name", 0));
	//	llp_out_message(lpm);
	llp_message_free(lpm);
	
	// test2 message
	lpm = llp_message_new(env, "test2");
	lpm2 = llp_message_new(env, "test2");
	llp_Wmes_int64(lpm, "id", 333);
	llp_Wmes_string(lpm, "name", "t2_name!");
	llp_Wmes_int32(lpm, "rgb", 444);
	for(i=0; i<LENS; i++)
		llp_Wmes_int32(lpm, "al", i+10);
	test2_p_mes(lpm);
	
	{		// Êä³ö
		llp_in_message(llp_out_message(lpm), lpm2);
		print("in-------\n");
		test2_p_mes(lpm2);
	}
	
	llp_message_free(lpm2);
	llp_message_free(lpm);
}

void at_test_read(llp_mes* lpm)
{
	unsigned int i=0;
	// --- read
	print("aa = %d\n", llp_Rmes_int32(lpm, "aa", 0));
	print("bb = %d\n", llp_Rmes_int32(lpm, "bb", 0));
	
	for(i=0; i<llp_Rmes_size(lpm, "cc"); i++)
		print("cc[%d] = %ld\n", i, llp_Rmes_int64(lpm, "cc", i));
	
	for(i=0; i<llp_Rmes_size(lpm, "dd"); i++)
	{
		llp_mes* lm = llp_Rmes_message(lpm, "dd", i);
		print("dd.tt = %d\n", llp_Rmes_int32(lm, "tt", 0));
		print("dd.tt2 = %d\n", llp_Rmes_int32(lm, "tt2", 0));
	}
}

void at_test(llp_env* env)
{
	unsigned int i=0;
	llp_mes* lpm = NULL;
	lpm = llp_message_new(env, "at");
	
	// write
	llp_Wmes_int32(lpm, "aa", 123);
	llp_Wmes_int32(lpm, "bb", 456);
	
	for(i=0; i<10; i++)
		llp_Wmes_int64(lpm, "cc", 34567*i);

	for(i=0; i<10; i++)
	{
		llp_mes* lm = llp_Wmes_message(lpm, "dd");
		llp_Wmes_int32(lm, "tt", 600*i);
		llp_Wmes_int32(lm, "tt2", 700*i);
	}
	
	at_test_read(lpm);
	{
		llp_mes* lo = llp_message_new(env, "at");
		llp_in_message(llp_out_message(lpm), lo);
		print("-----in\n");
		at_test_read(lo);
		llp_message_free(lo);
	}
	llp_message_free(lpm);
}

int main(void)
{
	llp_env* env = NULL;
	
	env = llp_new_env();
	
	llp_reg_mes(env, "test.mes.lpb");
	llp_reg_mes(env, "at.mes.lpb");
	
//	test2(env);
	at_test(env);

	llp_free_env(env);
	print("mem = %d\n", mem);
	return 0;
}