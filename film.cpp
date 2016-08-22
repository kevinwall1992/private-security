#include "Film.h"
#include "Common.h"
#include "Math.h"
#include "System.h"

#include "ISPCKernels.h"


Film::Film(int width_, int height_)
#if DRAW_DIRECTLY_TO_SCREEN
	: image()
#else
	: image(width_, height_)
#endif
{
	width= width_;
	height= height_;

#if SOA_RECEPTORS
	receptors_r= new float[width* height];
	receptors_g= new float[width* height];
	receptors_b= new float[width* height];
#else
	receptors= new Vec3f[width* height];
#endif

	Clear();
}

Film::~Film()
{
#if SOA_RECEPTORS
	delete receptors_r;
	delete receptors_g;
	delete receptors_b;
#else
	delete receptors;
#endif
}

void Film::Clear()
{
#if SOA_RECEPTORS
#if ISPC_CLEAR
	ispc::Clear(receptors_r, receptors_g, receptors_b, width* height);

#else
	for(int i= 0; i< width* height; i++)
	{
		receptors_r[i]= 0.0f;
		receptors_g[i]= 0.0f;
		receptors_b[i]= 0.0f;
	}
#endif

#else
	for(int i= 0; i< width* height; i++)
		receptors[i]= Vec3f(0, 0, 0);
#endif
}

bool Film::Clear_Parallel(int interval_index)
{
	if(interval_index>= (width* height)/ FILM_INTERVAL_SIZE)
		return false;

#if SOA_RECEPTORS
#if ISPC_CLEAR
	int interval_offset= interval_index* FILM_INTERVAL_SIZE;
	ispc::Clear(receptors_r+ interval_offset, 
		        receptors_g+ interval_offset, 
				receptors_b+ interval_offset, 
				FILM_INTERVAL_SIZE);

#else
	assert(false&& "Clear_Parallel not implement for non ISPC clear");
#endif

#else
	float *flattened_receptors= reinterpret_cast<float *>(receptors+ interval_index* FILM_INTERVAL_SIZE);
	std::fill(flattened_receptors, flattened_receptors+ FILM_INTERVAL_SIZE* 3, 0.0f);

#endif

	return true;
}

void Film::Stimulate(int x, int y, Color light)
{
#if SOA_RECEPTORS
	assert(false && "Stimulate() under SOA_RECEPTORS mode not implemented");
#else
	receptors[x+ width* y]+= light;
#endif
}

void Film::Develop()
{
#if ISPC_SHADING
	assert(false && "Single threaded Develop() under SOA_RECEPTORS mode not implemented");

#else
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
#endif
}

struct BGRAPixel
{
	unsigned char b, g, r, a;
};

bool Film::Develop_Parallel(int interval_index)
{
	if(interval_index>= (width* height)/ FILM_INTERVAL_SIZE)
		return false;

#if SOA_RECEPTORS
#if DRAW_DIRECTLY_TO_SCREEN
	Image image= System::graphics.GetWindowImage();
#endif

#if ISPC_DEVELOP
	int interval_offset= interval_index* FILM_INTERVAL_SIZE;
	ispc::Develop(receptors_r+ interval_offset, 
		           receptors_g+ interval_offset, 
				   receptors_b+ interval_offset, 
				   reinterpret_cast<int8_t *>(image.GetPixels()+ interval_offset), 
				   1* SAMPLES_PER_PIXEL, FILM_INTERVAL_SIZE);

#else
	Pixel *pixels= &(image.pixels[interval_index* FILM_INTERVAL_SIZE]);

	for(int i= 0; i< FILM_INTERVAL_SIZE; i++)
	{
		pixels[i].r= (unsigned char)(receptors_r[i]* 255);
		pixels[i].g= (unsigned char)(receptors_g[i]* 255);
		pixels[i].b= (unsigned char)(receptors_b[i]* 255);
	}

#endif
#if DRAW_DIRECTLY_TO_SCREEN
	System::graphics.ReturnWindowImage(image);
#endif

#else
#if ISPC_DEVELOP
	ispc::Develop(reinterpret_cast<float *>(receptors+ interval_index* FILM_INTERVAL_SIZE), 
		          reinterpret_cast<int8_t *>(image.pixels+ interval_index* FILM_INTERVAL_SIZE), 
				  1, FILM_INTERVAL_SIZE* 3);

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
#endif

	return true;
}