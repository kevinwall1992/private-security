#ifndef PS_FURNITURE
#define PS_FURNITURE


#include "Object.h"
#include "Movement.h"

class Furniture : public Object, public MovementEnabler, public DirectionBlocker
{
protected:
	virtual void LoadXML(TiXmlElement *xml_element);

public:
	Furniture();

	virtual vector<Move *> GetPotentialMoves(Tile *source);

	virtual string GetEntityDataFilename();
	virtual string GetEntityDataFolderName();

	virtual TiXmlElement * EncodeXML();
};

#endif