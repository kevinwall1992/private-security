#ifndef EBR_FILM
#define EBR_FILM

#include "Image.h"
#include "Color.h"


struct Film
{
	int width, height;
	Vec3f *receptors;
	Image image;

	Film(int width, int height);
	~Film();

	void Clear();
	void Stimulate(int x, int y, Color light);

	void Develop();
	bool Develop_Parallel(int interval_index);//hmmm
};

#endif
