#ifndef PS_COMPOSITING_CAMERA
#define PS_COMPOSITING_CAMERA

#include "RayCamera.h"


class CompositingCamera : public RayCameraBase
{
	ColorImage diffuse_map;
	ColorImage glossiness_map;
	ColorImage normal_map;
	DepthImage depth_map;

	bool initialized= false;

	void Initialize(int width, int height);
	void ResizeResizables(int width, int height);
	void Update();

	void GenerateMaps(Scene &scene, int width, int height);

public:
	CompositingCamera(float fov, Vec3f position);

	ColorImage GetDiffuseMap();
	ColorImage GetGlossinessMap();
	ColorImage GetNormalMap();
	DepthImage GetDepthMap();

	virtual Photo TakePhoto(Scene &scene, int width, int height);
};

#endif
