#ifndef PS_OBJECT
#define PS_OBJECT

#include "Common.h"
#include "Perceptible.h"
#include "Chronon.h"
#include "Encodable.h"
#include "Physical.h"


class Tile;

class Object : public Polymorphic, public Entity, public virtual Chronal, public virtual HasPose, public Physical
{
protected:
	string GetXMLElementName();
	virtual void LoadXML(TiXmlElement *xml_element);

public:
	Object();

	Tile *GetTile();

	virtual void Step(Chronons chronons);

	Vec3f GetPosition();
	float GetRotation();
	void SetPosition(Vec3f position);
	void SetRotation(float rotation);

	virtual TiXmlElement * EncodeXML();
	static Object * DecodeXML(TiXmlElement *xml_element);
};

#endif