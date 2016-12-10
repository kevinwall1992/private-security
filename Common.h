#include "EBRParameters.h"
#include "Macros.h"

#include <string>
#include <vector>
#include <iostream>
#include <assert.h>
#include <algorithm>

using std::string;
using std::vector;
using std::cout;
using std::endl;

#ifndef EBR_POLYMORPHIC
#define EBR_POLYMORPHIC
class PolymorphicBase
{
protected:
	virtual void PolymorphicFunction()= 0;
};

class Polymorphic : public PolymorphicBase
{
protected:
	void PolymorphicFunction() {};
};
#endif

#include "Utility.h"