#ifndef EBR_CAMERA
#define EBR_CAMERA

#include "Film.h"
#include "Shutter.h"
#include "Math.h"
#include "Ray.h"

struct Camera
{
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

	//Try using reference for completeray here

	//Is "Get" the best word here?
	bool GetRayPackets(CompleteRayPacket first_ray_packet, int tile_index, int *indices= nullptr, int index_count= 0);
	bool GetRays(CompleteRay first_ray, int tile_index, int *indices= nullptr);

	float * GetFilteringKernels();
	

	Image * TakePicture(Scene &scene);

private:
	Shutter shutter;

	float samples_x[MAX_SAMPLES_PER_PIXEL];//Might consider finding a better place for this
	float samples_y[MAX_SAMPLES_PER_PIXEL];
	float filtering_kernels[MAX_SAMPLES_PER_PIXEL* 9];
};

#endif