#ifndef PS_ENCODABLE
#define PS_ENCODABLE

#include "XMLUtility.h"

#include <tinyxml.h>
#include <string>
#include <typeinfo>
#include <map>

using std::string;


//This designs enforces single inheritance with regards to encoding
//IF this becomes a problem, just need a encoding function that 
//Takes an TiXmlElement as an argument.

class Encodable
{
	std::map<string, type_info> class_map;

protected:
	virtual string GetXMLElementName()= 0;
	virtual void LoadXML(TiXmlElement *xml_element)= 0;

	template<class T>
	static T * DecodeXML_Naive(TiXmlElement *xml_element)
	{
		T *t= new T();
		t->LoadXML(xml_element);

		return t;
	}

public:
	virtual TiXmlElement * EncodeXML();

	template<class T>
	static T * DecodeXML(TiXmlElement *xml_element)
	{
		return dynamic_cast<T *>(T::DecodeXML(xml_element));
	}

	template<class T>
	static T * Copy(T *encodable)
	{
		return DecodeXML<T>(encodable->EncodeXML());
	}
};

#endif