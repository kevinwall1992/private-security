#ifndef PS_ANIMATION
#define PS_ANIMATION

#include "Prop.h"

class Animation
{
	Mesh *mesh;

public:
	Animation(Mesh *mesh);

	Mesh * GetMesh();
};

#endif
