#ifndef PS_NINEPATCH
#define PS_NINEPATCH

#include "Pane.h"

//Consider making Ninepatch a type of Resource
class Ninepatch : public Pane
{
public:
	enum Roundness { Sharp, Normal, Soft };

private:
	ColorImage image;
	Texture texture;
	Roundness roundness;

	float GetCornerSize();

public:
	Ninepatch(Texture texture, Vec2f offset, Vec2f size, Roundness roundness);
	Ninepatch(string image_filename, Roundness roundness);

	virtual void Draw();
};

#endif
