#ifndef EBR_CAMERA
#define EBR_CAMERA

#include "Film.h"
#include "Shutter.h"
#include "Math.h"
#include "Transform.h"
#include "Property.h"
#include "PropertySpecializations.h"

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

class Camera
{
public:
	class RotationProperty : public Property<float>
	{
	public:
		enum Type {Pitch, Yaw, Roll};

	private:
		Camera *camera;
		Type type;

	public:
		RotationProperty(float *value_pointer, Camera *camera, Type type);

		using Property::operator=;
		RotationProperty & operator=(const RotationProperty &other);

		void Set(const float &value);
	};

	class PositionProperty : public Property<Vec3f>
	{
		Camera *camera;

	public:
		PositionProperty(Vec3f *value_pointer, Camera *camera);

		using Property::operator=;
		PositionProperty & operator=(const PositionProperty &other);

		void Set(const Vec3f &value);
	};

	class FOVProperty : public Property<float>
	{
		Camera *camera;

	public:
		FOVProperty(float *value_pointer, Camera *camera);

		using Property::operator=;
		FOVProperty & operator=(const FOVProperty &other);

		void Set(const float &value);
	};

private:

	Vec3f position;
	float pitch= 0, yaw= 0, roll= 0;
	float fov;

	bool is_orthographic;
	Vec3f focus= Vec3f(0, 0, 0);

	bool position_is_valid= false;
	bool rotation_is_valid= false;
	bool fov_is_valid= false;

	void GeneratePosition();
	void SetOrthographicFocus(Vec3f orthographic_focus);

protected:
	Transform GetDirectionTransform();

	Vec3f SnapToPixel(Vec3f position, Vec2i pixel_size);

	bool ValidatePosition();
	bool ValidateRotation();
	bool ValidateFOV();
	bool ValidateAllAttributes();

	//void SetOrthographicHorizontalSize(float size);

public:
	Camera(float fov, Vec3f position);
	Camera(float fov, Vec3f focus, float pitch, float yaw);

	PositionProperty Position;
	RotationProperty Pitch, Yaw, Roll;
	FOVProperty FOV;

	Vec3f GetForward();
	Vec3f GetRight();
	Vec3f GetUp();

	Vec3f GetViewPlaneU();
	Vec3f GetViewPlaneV();

	void UseOrthographicProjection();
	void UsePerspectiveProjection();

	bool IsOrthographic();
	bool IsPerspective();

	virtual void LookAt(Vec3f position);

	void AssumeOrientation(Camera &other);

	Transform GetTransform();
	Transform GetProjectedTransform(float aspect_ratio);

	Photo TakePhoto(Scene &scene, Vec2i size, Photo::Type type=Photo::Type::FullColor);
	virtual PhotoBook TakePhotos(Scene &scene, Vec2i size, Photo::Type types)= 0;

	friend RotationProperty;
	friend PositionProperty;
};



#endif