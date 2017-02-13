#include "SupportSystem.h"
#include "Common.h"

#ifdef _WIN32
//  Windows
#define cpuid(info, x)    __cpuidex(info, x, 0)

#else
//  GCC Intrinsics
#include <cpuid.h>
void cpuid(int info[4], int InfoType){
    __cpuid_count(InfoType, 0, info[0], info[1], info[2], info[3]);
}

#endif

void SupportSystem::Initialize()
{
	int info[4];
	cpuid(info, 0);
	int nIds = info[0];

	cpuid(info, 0x80000000);
	unsigned nExIds = info[0];

	//  Detect Features
	if (nIds >= 0x00000001)
	{
		cpuid(info,0x00000001);
		HW_MMX    = (info[3] & ((int)1 << 23)) != 0;
		HW_SSE    = (info[3] & ((int)1 << 25)) != 0;
		HW_SSE2   = (info[3] & ((int)1 << 26)) != 0;
		HW_SSE3   = (info[2] & ((int)1 <<  0)) != 0;

		HW_SSSE3  = (info[2] & ((int)1 <<  9)) != 0;
		HW_SSE41  = (info[2] & ((int)1 << 19)) != 0;
		HW_SSE42  = (info[2] & ((int)1 << 20)) != 0;
		HW_AES    = (info[2] & ((int)1 << 25)) != 0;

		HW_AVX    = (info[2] & ((int)1 << 28)) != 0;
		HW_FMA3   = (info[2] & ((int)1 << 12)) != 0;

		HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
	}
	if (nIds >= 0x00000007)
	{
		cpuid(info,0x00000007);
		HW_AVX2   = (info[1] & ((int)1 <<  5)) != 0;

		HW_BMI1        = (info[1] & ((int)1 <<  3)) != 0;
		HW_BMI2        = (info[1] & ((int)1 <<  8)) != 0;
		HW_ADX         = (info[1] & ((int)1 << 19)) != 0;
		HW_SHA         = (info[1] & ((int)1 << 29)) != 0;
		HW_PREFETCHWT1 = (info[2] & ((int)1 <<  0)) != 0;

		HW_AVX512F     = (info[1] & ((int)1 << 16)) != 0;
		HW_AVX512CD    = (info[1] & ((int)1 << 28)) != 0;
		HW_AVX512PF    = (info[1] & ((int)1 << 26)) != 0;
		HW_AVX512ER    = (info[1] & ((int)1 << 27)) != 0;
		HW_AVX512VL    = (info[1] & ((int)1 << 31)) != 0;
		HW_AVX512BW    = (info[1] & ((int)1 << 30)) != 0;
		HW_AVX512DQ    = (info[1] & ((int)1 << 17)) != 0;
		HW_AVX512IFMA  = (info[1] & ((int)1 << 21)) != 0;
		HW_AVX512VBMI  = (info[2] & ((int)1 <<  1)) != 0;
	}
	if (nExIds >= 0x80000001)
	{
		cpuid(info,0x80000001);
		HW_x64   = (info[3] & ((int)1 << 29)) != 0;
		HW_ABM   = (info[2] & ((int)1 <<  5)) != 0;
		HW_SSE4a = (info[2] & ((int)1 <<  6)) != 0;
		HW_FMA4  = (info[2] & ((int)1 << 16)) != 0;
		HW_XOP   = (info[2] & ((int)1 << 11)) != 0;
	}

	Print();
}

void SupportSystem::Terminate()
{

}

bool SupportSystem::SSE()
{
	return HW_SSE;
}

bool SupportSystem::SSE2()
{
	return HW_SSE2;
}

bool SupportSystem::SSE3()
{
	return HW_SSE3;
}

bool SupportSystem::SSSE3()
{
	return HW_SSSE3;
}

bool SupportSystem::SSE4_1()
{
	return HW_SSE41;
}

bool SupportSystem::SSE4_2()
{
	return HW_SSE42;
}

bool SupportSystem::SSE4a()
{
	return HW_SSE4a;
}

bool SupportSystem::AVX()
{
	return HW_AVX;
}

bool SupportSystem::AVX2()
{
	return HW_AVX2;
}

bool SupportSystem::AVX512()
{
	return HW_AVX512F;
}

const char * YN(bool value)
{
	if(value)
		return "Yes";
	else
		return "No";
}

void SupportSystem::Print()
{
	cout << "### SSE Support ###" << endl;
	cout << "SSE: " << YN(System::support.HW_SSE) << endl;
	cout << "SSE2: "<< YN(System::support.HW_SSE2) <<endl;
	cout << "SSE3: "<< YN(System::support.HW_SSE3) << endl;
	cout << "SSSE3: "<< YN(System::support.HW_SSSE3) << endl;
	cout << "SSE4.1: "<< YN(System::support.HW_SSE41) << endl;
	cout << "SSE4.2: "<< YN(System::support.HW_SSE42) << endl;
	cout << "SSE4a: "<< YN(System::support.HW_SSE4a) << endl << endl;

	cout << "### AVX Support ###" << endl;
	cout << "AVX: " << YN(System::support.HW_AVX) << endl;
	cout << "AVX2: " << YN(System::support.HW_AVX2) << endl;
	cout << "AVX512: " << YN(System::support.HW_AVX512F) << endl;
	if(System::support.HW_AVX512F)
	{
		cout << "AVX512BW: " << YN(System::support.HW_AVX512BW) << endl;
		cout << "AVX512CD: " << YN(System::support.HW_AVX512CD) << endl;
		cout << "AVX512DQ: " << YN(System::support.HW_AVX512DQ) << endl; 
		cout << "AVX512ER: " << YN(System::support.HW_AVX512ER) << endl; 
		cout << "AVX512IFMA: " << YN(System::support.HW_AVX512IFMA) << endl; 
		cout << "AVX512PF: " << YN(System::support.HW_AVX512PF) << endl; 
		cout << "AVX512VBMI: " << YN(System::support.HW_AVX512VBMI) << endl; 
		cout << "AVX512VL: " << YN(System::support.HW_AVX512VL) << endl;
	}
	cout << endl;
}