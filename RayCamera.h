#ifndef PS_RAY_CAMERA
#define PS_RAY_CAMERA

#include "Camera.h"
#include "Mutable.h"


class RayCameraBase : public Camera, public Updatable
{
public:
	RayCameraBase(float fov, Vec3f position);
	RayCameraBase(float fov, Vec3f focus, float pitch, float yaw);

	Ray GetRay(float x, float y);
};

class RayCamera : public RayCameraBase
{
	Shutter shutter;
	Film film;

	float default_samples_x[MAX_SAMPLES_PER_PIXEL];//Might consider finding a better place for this***
	float default_samples_y[MAX_SAMPLES_PER_PIXEL];
	float filtering_kernels[MAX_SAMPLES_PER_PIXEL* 9];

	float progressive_samples_x[MIN_SAMPLES_PER_PIXEL];//Might consider finding a better place for this
	float progressive_samples_y[MIN_SAMPLES_PER_PIXEL];

	//Consider naming changes that take into account the fact that***
	//GraphicsSystem also has a frame_count
	int current_frame_count= -1;
	int frame_count= 0;

	bool film_was_resized= true;

	void InitializeSamples();
	void GetSamples(float *&samples_x, float *&samples_y);

protected:
	void Update();

public:
	RayCamera(float fov, Vec3f position);
	RayCamera(float fov, Vec3f focus, float pitch, float yaw);

	void LookAt(Vec3f look_at_position);

	bool GetRayPackets(EBRRayPacket *ray_packets, int tile_index, int *indices= nullptr, int index_count= 0);
	bool GetRays(EBRRay *rays, int tile_index, int *indices= nullptr);

	float * GetFilteringKernels();

	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types);

	friend class Shutter;
};

#endif
