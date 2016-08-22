#ifndef EBR_FILM
#define EBR_FILM

#include "Common.h"
#include "Image.h"
#include "Color.h"



struct Film
{
	int width, height;
	Image image;

#if SOA_RECEPTORS
	float *receptors_r;
	float *receptors_g;
	float *receptors_b;
#else
	Vec3f *receptors;
#endif

	Film(int width, int height);
	~Film();

	void Clear();
	bool Clear_Parallel(int interval_index);

	void Stimulate(int x, int y, Color light);

	void Develop();
	bool Develop_Parallel(int interval_index);
};

#endif
