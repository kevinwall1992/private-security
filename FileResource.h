#ifndef PS_FILE_RESOURCE
#define PS_FILE_RESOURCE

#include "Resource.h"
#include "ResourceSystem.h"


template<class T>
class FileResource : public Resource
{
	string filepath;
	vector<string> element_path;

	static string MakeName(string filepath, vector<string> element_path)
	{
		string name= filepath;

		for(unsigned int i= 0; i< element_path.size(); i++)
			name+= ":"+ element_path[i];

		return name;
	}

	static string MakeName(string filepath, string element_name)
	{
		return filepath+ ":"+ element_name;
	}

protected:
	FileResource(string filepath, vector<string> element_path)
		: Resource(MakeName(filepath, element_path))
	{
		this->filepath= filepath;
		this->element_path= element_path;
	}

	FileResource(string filepath, string element_name)
		: Resource(MakeName(filepath, element_name))
	{
		this->filepath= filepath;
		this->element_path.push_back(element_name);
	}

	FileResource(string filepath)
		: Resource(filepath)
	{
		this->filepath= filepath;
	}

public:
	string GetFilepath()
	{
		return filepath;
	}

	vector<string> GetElementPath()
	{
		return element_path;
	}

	//Need some error handling
	//Would like to be able to get a Mesh given only a filename, to simplify case where file only has one object
	template<class... Args>
	static T * Retrieve(string filename, Args... element_path)
	{
		string filepath= T::MakeFilepath(filename);//This needs improving
		string name= MakeName(filepath, Utility::MakeVector<string>(element_path...));
		
		Resource *resource= System::resource.GetResource(name);
		if(resource!= nullptr)
			return dynamic_cast<T *>(resource);

		T::Parse(filename);//This should use filepath, especially if we keep MakeFilepath for use here
		return dynamic_cast<T *>(System::resource.GetResource(name));
	}
};

#endif
