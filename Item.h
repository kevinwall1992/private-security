#ifndef PS_ITEM
#define PS_ITEM


#include "Object.h"

class Item : public Object
{
public:
	Item();

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();
};

class Gun : public Item
{
public:
	Gun();

	virtual string GetEntityDataFilename();
};

#endif
