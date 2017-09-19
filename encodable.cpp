#include "Encodable.h"

TiXmlElement * Encodable::EncodeXML()
{
	return new TiXmlElement(GetXMLElementName().c_str());
}
