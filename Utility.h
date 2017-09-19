#ifndef EBR_UTILITY
#define EBR_UTILITY

#include "Common.h"
#include "Promote.h"

#include <map>


template<class Type, class RequiredType> 
struct Where
{
	static void constraints(Type* type) 
	{ 
		RequiredType* required_type = type; 
	}

	Where() 
	{ 
		void(*type)(Type*) = constraints; 
	}
};

namespace Utility
{
	namespace UtilityHelpers
	{
		template<class T, class... Arguments>
		void MakeVector(vector<T> &vector_, T element, Arguments... arguments)
		{
			vector_.push_back(element);
			MakeVector<T>(vector_, arguments...);
		}

		template<class T>
		void MakeVector(vector<T> &vector_, T element)
		{
			vector_.push_back(element);
		}
	}

	vector<string> TokenizeOverSpaces(string string_);

	bool FileExists(string filepath);

	vector<string> SplitString(string string_, char delimiter= '\n');

	string TextFileToString(string filepath);

	template<class T, class... Arguments>
	vector<T> MakeVector(T element, Arguments... arguments)
	{
		vector<T> vector_;
		UtilityHelpers::MakeVector<T>(vector_, element, arguments...);

		return vector_;
	}

	template<class T>
	vector<T> MakeVector(T element)
	{
		vector<T> vector_= {element};

		return vector_;
	}

	template<class T>
	vector<T> MakeVector()
	{
		return vector<T>();
	}

	template<class T>
	int GetIndexOfElement(vector<T> &vector_, T element)
	{
		for(unsigned int i= 0; i< vector_.size(); i++)
			if(element== vector_[i])
				return i;

		return -1;
	}

	template<class T>
	void RemoveElement(vector<T> &vector_, T element)
	{
		int index= GetIndexOfElement(vector_,element);
		if(index!= -1)
			vector_.erase(vector_.begin()+ index);
	}

	template<class T>
	void ReplaceElement(vector<T> &vector_, T element, T replacement)
	{
		int index= GetIndexOfElement(vector_, element);
		if(index!= -1)
		{
			vector_.erase(vector_.begin()+ index);
			vector_.insert(vector_.begin()+ index, replacement);
		}
	}

	template<class Key, class Value>
	bool Contains(const std::map<Key, Value> &container, const Key &key)
	{
		return container.find(key)!= container.end();
	}

	string Replace(string original, string to_replace, string replacement);
	string ToUnixFilepath(string filepath);
	string GetFilename(string filepath);
	string GetDirectoryPath(string filepath);

	template<class A, class B>
	bool IsType(B *object_)
	{
		return dynamic_cast<A *>(object_)!= nullptr;
	}

	template<class T>
	T StringToT(string value_string);

	template<>
	float StringToT<float>(string value_string);

	template<>
	int StringToT<int>(string value_string);
}

#endif