#ifndef PS_PERCEPTIBLE
#define PS_PERCEPTIBLE

#include "Common.h"
#include "Visible.h"
#include "Audible.h"
#include "Sound.h"
#include "Animation.h"
#include "HasPosition.h"
#include "FileResource.h"
#include "Pose.h"

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

class Entity: public Perceptible, public virtual HasPosition, public virtual HasPose
{
	struct EntityData : public FileResource<EntityData>
	{
		std::map<string, Animation *> animations;
		std::map<string, Sound> sounds;

		static string MakeFilepath(string filename);
		static vector<EntityData *> Parse(string filename);

	private:
		EntityData(string filepath);
	};

	MeshProp *mesh_prop= nullptr;

	EntityData * GetEntityData();

	MeshProp * GetMeshProp();

public:

	struct Event
	{
		enum Enum
		{
			Shoot, Explode
		};
	};

	Entity();

	virtual string GetEntityDataFilename()= 0;
	virtual string GetEntityDataFolderName()= 0;

	virtual vector<RaytracingPrimitive *> GetRaytracingPrimitives();
	virtual void Rasterize();
};

#endif
