#ifndef EBR_NAMED
#define EBR_NAMED

template<class NameType>
class Named
{
	virtual bool IsNamed(NameType &name)= 0;
};

#endif