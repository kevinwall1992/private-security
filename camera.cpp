#include "Camera.h"


Photo::Photo(ColorImage image_)
{
	image= image_;

	is_depth= false;
	image_was_first= true;
	image_is_empty= false;
	texture_is_empty= true;
}

Photo::Photo(DepthImage image_)
{
	depth_image= image_;

	is_depth= true;
	image_was_first= true;
	image_is_empty= false;
	texture_is_empty= true;
}

Photo::Photo(Texture texture_)
	: texture(texture_)
{
	is_depth= false;
	image_was_first= false;
	image_is_empty= true;
	texture_is_empty= false;
}

Photo::Photo(DepthTexture texture_)
	:texture(texture_)
{
	is_depth= true;
	image_was_first= false;
	image_is_empty= true;
	texture_is_empty= false;
}

Photo::Photo()
{
	image_is_empty= true;
	texture_is_empty= true;
}

Photo & Photo::operator=(const Photo &other)
{
	this->image= other.image;
	this->depth_image= other.depth_image;
	this->texture= other.texture;

	this->is_depth= other.is_depth;
	this->image_is_empty= other.image_is_empty;
	this->texture_is_empty= other.texture_is_empty;

	return *this;
}

Photo::Type Photo::GetType()
{
	return type;
}

ColorImage Photo::GetImage()
{
	if(!image_is_empty)
	{
		if(is_depth)
			assert(false && "Photo: DepthImage to ColorImage not implemented");
		else
			return image;
	}

	image.Size= texture.Size;
	texture.BindToIndex(0);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());

	return image;
}

Texture Photo::GetTexture()
{
	if(!texture_is_empty)
		return texture;

	if(is_depth)
		texture= DepthTexture(depth_image);
	else
		texture= Texture(image);
	texture_is_empty= false;

	return texture;
}

void Photo::Free()
{
	if(image_was_first)
	{
		if(!texture_is_empty)
			texture.Free();
	}
	else
	{
		if(!image_is_empty)
		{
			if(is_depth)
				depth_image.Free();
			else
				image.Free();
		}
	}
}


Transform Camera::GetDirectionTransform()
{
	return Transform().RotateAboutZ(Roll).RotateAboutX(Pitch).RotateAboutY(-Yaw);
}

Camera::Camera(float fov_, Vec3f position_)
	: Position(&position, this), 
	  Pitch(&pitch, this), 
	  Yaw(&yaw, this), 
	  Roll(&roll, this), 
	  FOV(&fov, this)
{
	Position= position_;
	FOV= fov_;
}

Vec3f Camera::GetForward()
{
	return GetDirectionTransform().Apply(Vec3f(0, 0, -1), true);
}

Vec3f Camera::GetRight()
{
	return GetDirectionTransform().Apply(Vec3f(1, 0, 0), true);
}

Vec3f Camera::GetUp()
{
	return GetDirectionTransform().Apply(Vec3f(0, 1, 0), true);
}

void Camera::LookAt(Vec3f look_at_position)
{
	Vec3f direction= (look_at_position- position).Normalized();

	Pitch= acos(Vec2f((float)sqrt(pow(direction.x, 2)+ pow(direction.z, 2)), direction.y).Normalize().Dot(Vec2f(1, 0)));
	if(direction.y< 0)
		Pitch= -Pitch;

	Yaw= -acos(Vec2f(direction.x, direction.z).Normalize().Dot(Vec2f(0, -1)));
	if(direction.x< 0)
		Yaw= -Yaw;

	//This is flipped to to support "lower yaw -> turn left"
	Yaw= -Yaw;

	Roll= 0;
}

void Camera::AssumeOrientation(Camera & other)
{
	Position= other.Position;
	Pitch= other.Pitch;
	Yaw= other.Yaw;
	Roll= other.Roll;
	FOV= other.FOV;
}

/*void Camera::OrthogonalizeVectors()
{
	up= Vec3f(0, 1, 0);
	forward.Normalize();

	right= forward.Cross(up);
	up= right.Cross(forward);
}*/

Transform Camera::GetTransform()
{
	return Transform().RotateAboutX(pitch).RotateAboutY(-yaw).Translate(position).Invert();
}

Transform Camera::GetProjectedTransform(float aspect_ratio)
{
	return GetTransform().Merge(Transform::MakeProjectionTransform(fov, aspect_ratio));
}

Photo Camera::TakePhoto(Scene & scene, Vec2i size, Photo::Type type)
{
	return TakePhotos(scene, size, type)[type];
}
