#include "lib_lp.h"
#include "lp_conf.h"
#include "lib_mes.h"
#include "lib_io.h"

#define LENS 4
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

int main(void)
{
	slice ins = {0};
	int i=0;
	llp_env env = {0};
	llp_mes* lpm = NULL;
	llp_mes* lpm2 = NULL;
	
	get_llp_env(&env);
	
	print("----reg before mem = %d \n", mem);
	llp_reg_mes(&env, "test.lpb");
	print("-----reg after mem = %d\n", mem);
	
	// test1 message 
	lpm = llp_message_new(&env, "test1");		// create a message obj
	llp_Wmes_int32(lpm, "a", 1523698);
	llp_Wmes_int32(lpm, "b", 456);
	llp_Wmes_string(lpm, "name", "test_name~~");
	
	print("test1 a= %d \n", llp_Rmes_int32(lpm, "a", 0));
	print("test1 b = %d \n", llp_Rmes_int32(lpm, "b", 0));
	print("test1 name = %s \n", llp_Rmes_string(lpm, "name", 0));
	//	llp_out_message(lpm);
	llp_message_free(lpm);
	
	
	// test2 message
	lpm = llp_message_new(&env, "test2");
	lpm2 = llp_message_new(&env, "test2");
	llp_Wmes_int64(lpm, "id", 333);
	llp_Wmes_string(lpm, "name", "t2_name!");
	llp_Wmes_int32(lpm, "rgb", 444);
	for(i=0; i<LENS; i++)
		llp_Wmes_int32(lpm, "al", i+10);
	test2_p_mes(lpm);
	
	{		// Êä³ö
		llp_out_message(lpm);
		ins.sp = lpm->sio.b_sp;
		ins.b_sp = lpm->sio.b_sp;
		ins.sp_size = lpm->sio.sp - lpm->sio.b_sp;
		llp_in_message(&ins, lpm2);
		print("in-------\n");
		test2_p_mes(lpm2);
	}

	llp_message_free(lpm2);
	llp_message_free(lpm);
	
	free_llp_env(&env);
	print("mem = %d\n", mem);
	return 0;
}