#ifndef PS_TREELIKE
#define PS_TREELIKE

template<class T>
class Treelike
{
	T *parent= nullptr;

public:

	virtual void SetParent(T *parent)
	{
		this->parent= parent;
	}

	T * GetParent()
	{
		return parent;
	}
};

#endif
