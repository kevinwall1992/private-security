#ifndef PS_PERCEPTIBLE
#define PS_PERCEPTIBLE

#include "Common.h"
#include "Visible.h"
#include "Audible.h"
#include "Sound.h"
#include "Animation.h"
#include "HasOrientation.h"
#include "FileResource.h"
#include "Pose.h"
#include "Encodable.h"

#include <map>


class Perceptible: public Prop, public Audible
{
public:

};

//Would like to somehow reuse PropContainer functionality
class PerceptibleContainer: public Perceptible
{
public:
	virtual vector<Perceptible *> GetPerceptibles()= 0;

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
};


//One size fits all class for game objects. 
//Renders them based off of provided data files and
//state info (Is the object open/closed, standing/walking/etc, 
//At what point in that state are they)
//Also handles making sounds based on events
//May possibly integrate events into visuals in the future

//The confusion due to Entity/Component/System model may mean this needs name change

class Entity: public Perceptible, public virtual HasOrientation, public virtual HasPose, public Encodable
{
	struct EntityData
	{
		std::map<string, Mesh *> meshes;
		std::map<string, Animation *> animations;
		std::map<string, Sound> sounds;

		EntityData();
	};
	EntityData entity_data;

	Mesh * GetDefaultMesh();
	Animation * GetAnimation();

protected:
	virtual void LoadXML(TiXmlElement *xml_element);

public:

	struct Event
	{
		enum Enum
		{
			Shoot, Explode
		};
	};

	Entity();

	//This is just quick and dirty. Will expand Icon functionality 
	//(and Perceptible querying in general) later ***
	Mesh * GetIconMesh();
	Mesh * GetMesh(string name);
	std::map<string, Mesh *> GetMeshes();
	void SetMesh(string name, Mesh *mesh);

	//virtual string GetEntityDataFilename()= 0;
	//virtual string GetEntityDataFolderName()= 0;
	virtual string GetEntityAnimationName();
	virtual float GetEntityAnimationMoment();

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();

	virtual TiXmlElement * EncodeXML();
};

#endif
