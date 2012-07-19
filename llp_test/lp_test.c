#include "./llp.h"

#pragma comment(lib, "../llp/sllp/Debug/sllp.lib")

void test(llp_env* env)
{
	int i=0;
	slice* p;
	byte data[] = {1, 2, 3, 4, 5, 6};
	slice sl = {data, data, sizeof(data)};
	llp_mes* lm = llp_message_new(env, "testM");
	llp_mes* lms = llp_message_new(env, "testM");

	llp_Wmes_integer(lm, "aa", 123456);
	llp_Wmes_string(lm, "bb", "test_string");
	llp_Wmes_real(lm, "cc", 345.67124);
	llp_Wmes_bytes(lm, "dd", &sl);


	llp_in_message(llp_out_message(lm), lms);
	print("%d\n%s\n%lf",
			(int)llp_Rmes_integer(lms, "aa", 0),
			llp_Rmes_string(lms, "bb", 0),
			llp_Rmes_real(lms, "cc", 0)
		);

	p=llp_Rmes_bytes(lms, "dd", 0);
	for(i=0; i<p->sp_size; i++){
		print("%d ", p->sp[i]);
	}

	llp_message_free(lm);
	llp_message_free(lms);
}

int main(void)
{
	llp_env* env = NULL;
	env = llp_new_env();

	llp_reg_mes(env, "test.mes.lpb");
	
	test(env);

	llp_free_env(env);
	print_mem();
	getchar();
	return 0;
}