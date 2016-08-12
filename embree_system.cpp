#include "EmbreeSystem.h"
#include "Common.h"


void ErrorHandler(const RTCError code, const char* str)
{
	if(code== RTC_NO_ERROR)
		return;

	cout << "Error: " << code << "; " << str << endl;
}

void EmbreeSystem::Initialize()
{
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	device = rtcNewDevice(nullptr);
	ErrorHandler(rtcDeviceGetError(device), "creation");
	rtcDeviceSetErrorFunction(device,ErrorHandler);
}

void EmbreeSystem::Terminate()
{
	rtcDeleteDevice(device);
}