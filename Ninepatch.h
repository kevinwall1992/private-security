#ifndef PS_NINEPATCH
#define PS_NINEPATCH

#include "Pane.h"

class Ninepatch : public Pane
{
public:
	enum Roundness { Sharp, Normal, Soft };

private:
	Texture texture;
	float corner_size;

	void SetRoundness(Roundness roundness);

public:
	Ninepatch(Texture texture, Vec2f offset, Vec2f size, Roundness roundness);
	Ninepatch(string image_filename, Roundness roundness);

	virtual void Draw();
};

#endif
