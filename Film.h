#ifndef EBR_FILM
#define EBR_FILM

#include "Common.h"
#include "Image.h"
#include "Color.h"



struct Film : public Resizable
{
	ColorImage image;

	float *receptors_r;
	float *receptors_g;
	float *receptors_b;
	int *sample_counts;

	Film(int width, int height);
	Film();
	~Film();

	void Resize(int width, int height);

	void Clear();
	bool Clear_Parallel(int interval_index);

	void Stimulate(int x, int y, Color light);

	void Develop(float factor);
	bool Develop_Parallel(float factor, int interval_index);

private:
	void Initialize_Sized();
	void Free_Sized();
};

#endif
