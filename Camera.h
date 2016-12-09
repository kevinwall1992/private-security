#ifndef EBR_CAMERA
#define EBR_CAMERA

#include "Film.h"
#include "Shutter.h"
#include "Math.h"
#include "Ray.h"

class Camera
{
	Shutter shutter;

	float default_samples_x[MAX_SAMPLES_PER_PIXEL];//Might consider finding a better place for this
	float default_samples_y[MAX_SAMPLES_PER_PIXEL];
	float filtering_kernels[MAX_SAMPLES_PER_PIXEL* 9];

	float progressive_samples_x[MIN_SAMPLES_PER_PIXEL];//Might consider finding a better place for this
	float progressive_samples_y[MIN_SAMPLES_PER_PIXEL];
	int current_frame_count= -1;

	void GetSamples(float *&samples_x, float *&samples_y);

public:

	Vec3f position, forward, up, right;
	float fov;
	bool orthograhpic;

	Vec3f view_plane_u;
	Vec3f view_plane_v;

	Film *film;


	Camera(float fov_in_degrees, Vec3f position, Vec3f direction= Vec3f(0, 0, -1));

	void LookAt(Vec3f look_at_position);
	void OrthogonalizeVectors();
	void ComputeViewPlane();

	Film * LoadFilm(Film *film);
	Film * RemoveFilm();

	Shutter * GetShutter();

	//Is "Get" the best word here?
	bool GetRayPackets(RayPacket *ray_packets, int tile_index, int *indices= nullptr, int index_count= 0);
	bool GetRays(Ray *rays, int tile_index, int *indices= nullptr);

	float * GetFilteringKernels();
	

	Image * TakePicture(Scene &scene);
};

#endif