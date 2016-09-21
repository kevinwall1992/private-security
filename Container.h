#ifndef EBR_CONTAINER
#define EBR_CONTAINER


template<class T>
class Iterator
{
public:
	virtual T GetNext()= 0;
	virtual void Reset()= 0;
};

template<class T>
class Container
{
public:
	virtual Iterator<T> * GetIterator()= 0;
};

#endif