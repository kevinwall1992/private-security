#ifndef PS_OBJECT
#define PS_OBJECT

#include "Common.h"
#include "Perceptible.h"


//Considering whether subclasses of Actor, Furniture, etc might simply be 
//offloaded into data files... (Certainly we weren't planning on subclassing 
//for every single Actor with a different haircolor, but we still have classes 
//specifying their own data files currently)
//Makes the relationship with the graphical and ui layer much more elegant. 
//Can still derive subclasses for special functionality; data files just 
//specify what subclass they are. 

class Tile;

class Object : public Polymorphic, public Entity
{
public:

	Object();

	Tile *GetTile();

	Vec3f GetPosition();

	virtual Pose GetPose();
};

#endif