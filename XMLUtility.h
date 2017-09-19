#ifndef XML_UTILITY
#define XML_UTILITY

#include "Vector.h"
#include <tinyxml.h>
#include <string>


using std::string;

namespace Utility
{
	namespace XML
	{
		TiXmlElement * MakeElementWithText(string name, string text);
		void AddOrReplaceChildElement(TiXmlElement *parent, TiXmlElement *child);

		TiXmlElement * EncodeVector(Vec3f vector, string name);
		TiXmlElement * EncodeVector(Vec2f vector, string name);
		TiXmlElement * EncodeVector(Vec3i vector, string name);
		TiXmlElement * EncodeVector(Vec2i vector, string name);

		Vec3f DecodeVec3f(TiXmlElement *xml_vector);
		Vec2f DecodeVec2f(TiXmlElement *xml_vector);
		Vec3i DecodeVec3i(TiXmlElement *xml_vector);
		Vec2i DecodeVec2i(TiXmlElement *xml_vector);
	}
}

#endif

