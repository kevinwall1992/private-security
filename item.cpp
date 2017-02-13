#include "Item.h"

Item::Item()
{
}

string Item::GetEntityDataFilename()
{
	return "item.xml";
}

string Item::GetEntityDataFolderName()
{
	return "items";
}

Gun::Gun()
{
}

string Gun::GetEntityDataFilename()
{
	return "gun.xml";
}
