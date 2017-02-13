#ifndef PS_DICTIONARY_FILE
#define PS_DICTIONARY_FILE

#include "Dictionary.h"
#include "FileResource.h"

class DictionaryFile : public Dictionary, public FileResource<DictionaryFile>
{
protected:
	DictionaryFile(string filepath);

public:
	static string MakeFilepath(string filename);
	static vector<DictionaryFile *> Parse(string filename);
};

#endif
