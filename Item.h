#ifndef PS_ITEM
#define PS_ITEM


#include "Object.h"

class Item : public Object
{
protected:
	virtual void LoadXML(TiXmlElement *xml_element);

public:
	Item();

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();

	virtual TiXmlElement * EncodeXML();
};

class Gun : public Item
{
protected:
	virtual void LoadXML(TiXmlElement *xml_element);

public:
	Gun();

	virtual string GetEntityDataFilename();

	virtual TiXmlElement * EncodeXML();
};

#endif
