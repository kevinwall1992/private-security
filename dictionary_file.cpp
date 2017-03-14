#include "DictionaryFile.h"

#include <tinyxml.h>


string DictionaryFile::MakeFilepath(string filename)
{
	return "data/"+ filename;
}

vector<DictionaryFile*> DictionaryFile::Parse(string filename)
{
	return Utility::MakeVector(new DictionaryFile(MakeFilepath(filename)));
}

DictionaryFile::DictionaryFile(string filepath)
	: FileResource(filepath)
{
	TiXmlDocument dictionary_file;
	dictionary_file.LoadFile(filepath.c_str());

	TiXmlElement *element= dictionary_file.RootElement();
	while(element!= nullptr)
	{
		if(element->ValueTStr()== "dictionary")
			element= element->FirstChildElement();

		if(element->ValueTStr()== "element")
			(*this)[element->FirstChildElement("key")->GetText()]= element->FirstChildElement("value")->GetText();

		element= element->NextSiblingElement();
	}
}

