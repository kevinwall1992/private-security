#include "XMLUtility.h"
#include "Utility.h"


TiXmlElement * Utility::XML::MakeElementWithText(string name, string text)
{
	TiXmlElement *xml_element= new TiXmlElement(name.c_str());
	TiXmlText *xml_element_text= new TiXmlText(text.c_str());
	xml_element->LinkEndChild(xml_element_text);

	return xml_element;
}

void Utility::XML::AddOrReplaceChildElement(TiXmlElement *parent, TiXmlElement *child)
{
	TiXmlElement *existing_child= parent->FirstChildElement(child->Value());

	if(existing_child== nullptr)
		parent->LinkEndChild(child);
	else
	{
		parent->RemoveChild(existing_child);
		parent->LinkEndChild(child);
	}
}

//For some reason, implementing this as a simple global variable doesn't work... in Release...?
vector<string> GetDefaultVectorComponentNames()
{
	static vector<string> default_vector_component_names= Utility::MakeVector<string>("x", "y", "z", "w");

	return default_vector_component_names;
}

template<int d, class T>
TiXmlElement * EncodeVector_(Vector<d, T> vector_, string name)
{
	TiXmlElement *xml_vector= new TiXmlElement(name.c_str());

	for(int i= 0; i< d; i++)
		xml_vector->LinkEndChild(Utility::XML::MakeElementWithText(GetDefaultVectorComponentNames()[i], std::to_string(vector_[i])));

	return xml_vector;
}

template<int d, class T>
Vector<d, T> DecodeVector(TiXmlElement *xml_vector)
{
	Vector<d, T> vector_;

	for(int i= 0; i< d; i++)
		vector_[i]= Utility::StringToT<T>(xml_vector->FirstChildElement(GetDefaultVectorComponentNames()[i].c_str())->GetText());

	return vector_;
}

TiXmlElement * Utility::XML::EncodeVector(Vec3f vector, string name)
{
	return EncodeVector_(vector, name);
}

TiXmlElement * Utility::XML::EncodeVector(Vec2f vector, string name)
{
	return EncodeVector_(vector, name);
}

TiXmlElement * Utility::XML::EncodeVector(Vec3i vector, string name)
{
	return EncodeVector_(vector, name);
}

TiXmlElement * Utility::XML::EncodeVector(Vec2i vector, string name)
{
	return EncodeVector_(vector, name);
}

Vec3f Utility::XML::DecodeVec3f(TiXmlElement *xml_vector)
{
	return DecodeVector<3, float>(xml_vector);
}

Vec2f Utility::XML::DecodeVec2f(TiXmlElement *xml_vector)
{
	return DecodeVector<2, float>(xml_vector);
}

Vec3i Utility::XML::DecodeVec3i(TiXmlElement *xml_vector)
{
	return DecodeVector<3, int>(xml_vector);
}

Vec2i Utility::XML::DecodeVec2i(TiXmlElement *xml_vector)
{
	return DecodeVector<2, int>(xml_vector);
}