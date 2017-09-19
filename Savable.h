#ifndef PS_SAVABLE
#define PS_SAVABLE

#include <string>

using std::string;


class Savable
{
	string filepath;

public:
	virtual void Save(string filepath);
	virtual void Load(string filepath);

	string GetFilepath();
	string GetFilename();
	string GetDirectoryPath();
};

#endif
