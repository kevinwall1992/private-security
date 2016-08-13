#ifndef EBR_TEST
#define EBR_TEST

class Test
{

protected:
	virtual void Execute()= 0;

public:
	Test();

	virtual float RunTest(int sample_count= 1);

	static float Compare(Test *test_a, Test *test_b, int set_size, int set_count, bool strided= true);
};


typedef void (*TestFunction)(int, void *);

class FunctionTest : public Test
{
	TestFunction test_function= nullptr;
	void *argument= nullptr;

protected:
	virtual void Execute();

public:
	FunctionTest(TestFunction test_function, void *argument);

	float RunTest(int sample_count= 1);
};

#endif