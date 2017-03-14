#ifndef PS_COMPOSITING_CAMERA
#define PS_COMPOSITING_CAMERA

#include "RayCamera.h"


class CompositingCamera : public RayCameraBase
{
	ColorImage diffuse_color_map;
	ColorImage glossiness_map;
	ColorImage normal_map;
	DepthImage depth_map;

	bool initialized= false;

	void Initialize(Vec2i size);
	void ResizeResizables(Vec2i size);
	void Update();

	void GenerateMaps(Scene &scene, Vec2i size);

public:
	CompositingCamera(float fov, Vec3f position);

	ColorImage GetDiffuseMap();
	ColorImage GetGlossinessMap();
	ColorImage GetNormalMap();
	DepthImage GetDepthMap();

	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types);
};

#endif
