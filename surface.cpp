#include "Surface.h"


Surface::Surface(Vec3f position_, Vec3f normal_, Material *material_)
{
	position= position_;
	normal= normal_;
	material= material_;
}

Surface::Surface()
{

}