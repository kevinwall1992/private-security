#ifndef EBR_UTILITY
#define EBR_UTILITY

#include "Common.h"


vector<string> TokenizeOverSpaces(string string_);

template<class Type, class RequiredType> 
struct Where
{
    static void constraints(Type* type) 
	{ 
		RequiredType* required_type = type; 
	}

    Where() 
	{ 
		void(*type)(Type*) = constraints; 
	}
};

#endif