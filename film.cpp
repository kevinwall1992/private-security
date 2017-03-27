#ifdef USE_EBR

#include "EBRParameters.h"
#include "Film.h"
#include "Common.h"
#include "Math.h"
#include "GraphicsSystem.h"
#include "Timer.h"

#include "ISPCKernels.h"


Film::Film(int width, int height)
{
	Resize(Vec2i(width, height));
}

Film::Film(Vec2i size)
{
	Resize(size);
}

Film::Film()
{
	receptors_r= nullptr;
	receptors_g= nullptr;
	receptors_b= nullptr;
	sample_counts= nullptr;
}

Film::~Film()
{
	Free();
	image.Free();
}

void Film::Resize(Vec2i size)
{
	if(size== Size)
		return;

	Free();

	receptors_r= new float[Width* Height];
	receptors_g= new float[Width* Height];
	receptors_b= new float[Width* Height];
	sample_counts= new int[Width* Height];

	Clear();

	image.Size= size;
}

void Film::Clear()
{
#if ISPC_CLEAR
	ispc::Clear(receptors_r, receptors_g, receptors_b, Width* Height);

#else
	for(int i= 0; i< width* height; i++)
	{
		receptors_r[i]= 0.0f;
		receptors_g[i]= 0.0f;
		receptors_b[i]= 0.0f;
	}
#endif

}

bool Film::Clear_Parallel(int interval_index)
{
	if(interval_index>= (Width* Height)/ FILM_INTERVAL_SIZE)
		return false;

#if ISPC_CLEAR
	int interval_offset= interval_index* FILM_INTERVAL_SIZE;
	ispc::Clear(receptors_r+ interval_offset, 
		        receptors_g+ interval_offset, 
				receptors_b+ interval_offset, 
				FILM_INTERVAL_SIZE);

#else
	assert(false&& "Clear_Parallel not implement for non ISPC clear");

#endif

	return true;
}

void Film::Stimulate(int x, int y, Color light)
{
	receptors_r[y* Width+ x]+= light[0];
	receptors_g[y* Width+ x]+= light[1];
	receptors_b[y* Width+ x]+= light[2];
}

void Film::Develop(float factor)
{
#if ISPC_SHADING
	assert(false && "Single threaded Develop() not implemented");

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

bool Film::Develop_Parallel(float factor, int interval_index)
{
	if(interval_index>= (Width* Height)/ FILM_INTERVAL_SIZE)
		return false;

	Timer::develop_timer.Start();

	//Image image= System::graphics.CheckOutWindowImage();

#if ISPC_DEVELOP
	int interval_offset= interval_index* FILM_INTERVAL_SIZE;
	ispc::Develop(receptors_r+ interval_offset, 
		           receptors_g+ interval_offset, 
				   receptors_b+ interval_offset, 
				   sample_counts+ interval_offset, 
				   reinterpret_cast<int8_t *>(image.GetPixels()+ interval_offset), //this should probably be uint8_t
				   0.95f/ factor, 
				   FILM_INTERVAL_SIZE);

#else
	Pixel *pixels= &(image.pixels[interval_index* FILM_INTERVAL_SIZE]);

	for(int i= 0; i< FILM_INTERVAL_SIZE; i++)
	{
		pixels[i].r= (unsigned char)(receptors_r[i]* 255);
		pixels[i].g= (unsigned char)(receptors_g[i]* 255);
		pixels[i].b= (unsigned char)(receptors_b[i]* 255);
	}

#endif

	//System::graphics.ReturnWindowImage(image);

	Timer::develop_timer.Pause();

	return true;
}

void Film::Free()
{
	if(receptors_r== nullptr)
		return;

	delete receptors_r;
	delete receptors_g;
	delete receptors_b;
	delete sample_counts;

	receptors_r= nullptr;
	receptors_g= nullptr;
	receptors_b= nullptr;
	sample_counts= nullptr;
}

#endif