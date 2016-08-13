#include "Test.h"
#include "Timer.h"


Test::Test()
{

}

float Test::RunTest(int sample_count)
{
	Timer timer;
	timer.Start();

	for(int i= 0; i< sample_count; i++)
		Execute();

	return timer.Stop();
}

float Test::Compare(Test *test_a, Test *test_b, int set_size, int set_count, bool strided)
{
	float test_a_elapsed_seconds= 0;
	float test_b_elapsed_seconds= 0;

	if(strided)
	{
		int current_set_count= 0;
		while(current_set_count< set_count)
		{
			test_a_elapsed_seconds+= test_a->RunTest(set_size);
			test_b_elapsed_seconds+= test_b->RunTest(set_size);
		}
	}
	else
	{
		test_a_elapsed_seconds= test_a->RunTest(set_size* set_count);
		test_b_elapsed_seconds= test_b->RunTest(set_size* set_count);
	}

	return test_a_elapsed_seconds- test_b_elapsed_seconds;
}


void FunctionTest::Execute()
{
	test_function(1, argument);
}

FunctionTest::FunctionTest(TestFunction test_function_, void *argument_)
{
	test_function= test_function_;
	argument= argument_;
}

float FunctionTest::RunTest(int sample_count)
{
	Timer timer;
	timer.Start();

	test_function(sample_count, argument);

	return timer.Stop();
}