#ifndef EBR_DATA
#define EBR_DATA

#include "Common.h"
#include "ISPCKernels.h"

struct ISPCData : public ispc::Data
{
	ISPCData(int max_count= 100000);

	void Write();
};

#endif