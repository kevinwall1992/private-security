#include "Film.h"
#include "Common.h"
#include "Math.h"

#include "ISPCKernels.h"


Film::Film(int width_, int height_)
	: image(width_, height_)
{
	width= width_;
	height= height_;

	receptors= new Vec3f[width* height];
	Clear();
}

Film::~Film()
{
	delete receptors;
}

void Film::Clear()
{
	for(int i= 0; i< width* height; i++)
		receptors[i]= Vec3f(0, 0, 0);
}

void Film::Stimulate(int x, int y, Color light)
{
	receptors[x+ width* y]+= light;
}

void Film::Develop()
{
#if ISPC_DEVELOP
	ispc::Develop(reinterpret_cast<float *>(receptors), reinterpret_cast<int8_t *>(image.pixels), 1, width* height* 3);

#else
	for(int i= 0; i< width* height; i++)
	{
		image.pixels[i].r= (unsigned char)((receptors[i].x)* 255);
		image.pixels[i].g= (unsigned char)((receptors[i].y)* 255);
		image.pixels[i].b= (unsigned char)((receptors[i].z)* 255);
	}

#endif
}

bool Film::Develop_Parallel(int interval_index)
{
	if(interval_index>= (width* height)/ FILM_INTERVAL_SIZE)
		return false;

#if ISPC_DEVELOP
	ispc::Develop(reinterpret_cast<float *>(receptors+ interval_index* FILM_INTERVAL_SIZE), reinterpret_cast<int8_t *>(image.pixels+ interval_index* FILM_INTERVAL_SIZE), 1, FILM_INTERVAL_SIZE* 3);

#else
	Pixel *pixels= &(image.pixels[interval_index* FILM_INTERVAL_SIZE]);
	Vec3f *receptors= &(this->receptors[interval_index* FILM_INTERVAL_SIZE]);

	for(int i= 0; i< FILM_INTERVAL_SIZE; i++)
	{
		pixels[i].r= (unsigned char)(receptors[i].x* 255);
		pixels[i].g= (unsigned char)(receptors[i].y* 255);
		pixels[i].b= (unsigned char)(receptors[i].z* 255);
	}

#endif

	return true;
}