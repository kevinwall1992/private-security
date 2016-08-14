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
	bool GetRays(CompleteRay first_ray, int tile_index);
	bool GetRayPackets(CompleteRayPacket first_ray_packet, int tile_index);

	Image *TakePicture(Scene &scene);

private:
	Shutter shutter;

};

#endif