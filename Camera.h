#ifndef EBR_CAMERA
#define EBR_CAMERA

#include "Film.h"
#include "Shutter.h"
#include "Math.h"
#include "Ray.h"
#include "Transform.h"
#include "Property.h"
#include "PropertySpecializations.h"


class Photo
{
	ColorImage image;
	DepthImage depth_image;
	Texture texture;

	bool is_depth;
	bool image_was_first;
	bool image_is_empty;
	bool texture_is_empty;

public:
	Photo(ColorImage image);
	Photo(DepthImage image);
	Photo(Texture texture);
	Photo(DepthTexture texture);

	Photo();

	Photo & operator=(const Photo &other);

	ColorImage GetImage();
	Texture GetTexture();

	void Free();
};


class Camera : public Updatable
{
	Vec3f position;
	float pitch= 0, yaw= 0, roll= 0;
	float fov;

	bool orthograhpic= false;

protected:
	Transform GetDirectionTransform();

public:
	Camera(float fov, Vec3f position);

	Vec3fProperty Position;
	FloatProperty Pitch, Yaw, Roll;
	FloatProperty FOV;

	Vec3f GetForward();
	Vec3f GetRight();
	Vec3f GetUp();

	virtual void LookAt(Vec3f position);

	void AssumeOrientation(Camera &other);

	Transform GetTransform();
	Transform GetProjectedTransform(int width, int height);
	
	virtual Photo TakePhoto(Scene &scene, int width, int height)= 0;
};



#endif