#include "Savable.h"
#include "Utility.h"


void Savable::Save(string filepath_)
{
	filepath= filepath_;
}

void Savable::Load(string filepath_)
{
	filepath= filepath_;
}

string Savable::GetFilepath()
{
	return filepath;
}

string Savable::GetFilename()
{
	return Utility::GetFilename(filepath);
}

string Savable::GetDirectoryPath()
{
	return Utility::GetDirectoryPath(filepath);
}
