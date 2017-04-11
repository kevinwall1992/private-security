#ifndef EBR_CAMERA
#define EBR_CAMERA

#include "Film.h"
#include "Shutter.h"
#include "Math.h"
#include "Transform.h"
#include "Property.h"
#include "PropertySpecializations.h"
#include "Mutable.h"

#include <map>


//Consider having static texture handle lying around***
//For use in uploading image to GPU. At least as fast
//As the current method, plus no blocking commands

//Or, rewrite to be less hand-wavey; just allow it to be
//created with an image or texture, then provide methods
//that inform caller what the underlying type is. 
class Photo
{
public:
	enum Type { FullColor= 1, 
		        DiffuseColor= 2, 
				SpecularColor= 4, 
				Depth= 8, 
				Normal= 16 };

private:
	ColorImage image;
	DepthImage depth_image;
	Texture texture;

	bool is_depth;
	bool image_was_first;
	bool image_is_empty;
	bool texture_is_empty;

	Type type;

public:
	Photo(ColorImage image);
	Photo(DepthImage image);
	Photo(Texture texture);
	Photo(DepthTexture texture);

	Photo();

	Photo & operator=(const Photo &other);

	Type GetType();

	ColorImage GetImage();
	Texture GetTexture();

	void Free();
};
typedef std::map<Photo::Type, Photo> PhotoBook;

class Camera : public Updatable
{
	static float orthographic_horizontal_size;

	Vec3f position;
	float pitch= 0, yaw= 0, roll= 0;
	float fov;

	bool is_orthographic= false;
	Vec3f orthographic_focus= Vec3f(0, 0, 0);

protected:
	Transform GetDirectionTransform();

public:
	Camera(float fov, Vec3f position);

	Vec3fSensorProperty Position;
	FloatSensorProperty Pitch, Yaw, Roll;
	FloatSensorProperty FOV;

	Vec3f GetForward();
	Vec3f GetRight();
	Vec3f GetUp();

	float GetOrthographicHorizontalSize();

	void UseOrthographicProjection();
	void UsePerspectiveProjection();

	bool IsOrthographic();
	bool IsPerspective();

	virtual void LookAt(Vec3f position);

	void AssumeOrientation(Camera &other);

	void Translate(Vec3f displacement);

	Transform GetTransform();
	Transform GetProjectedTransform(float aspect_ratio);

	void Update();
	
	Photo TakePhoto(Scene &scene, Vec2i size, Photo::Type type=Photo::Type::FullColor);
	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types)= 0;

	
};



#endif