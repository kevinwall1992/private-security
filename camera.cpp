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


float Camera::orthographic_horizontal_size= 5;

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
	position= position_;
	FOV= fov_;

	Camera::Update();
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

float Camera::GetOrthographicHorizontalSize()
{
	return orthographic_horizontal_size;
}

void Camera::UseOrthographicProjection()
{
	is_orthographic= true;
}

void Camera::UsePerspectiveProjection()
{
	is_orthographic= false;
}

bool Camera::IsOrthographic()
{
	return is_orthographic;
}

bool Camera::IsPerspective()
{
	return !is_orthographic;
}

void Camera::LookAt(Vec3f look_at_position)
{
	orthographic_focus= look_at_position;
	Camera::Update();

	Vec3f direction= (look_at_position- position).Normalized();

	pitch= acos(Vec2f((float)sqrt(pow(direction.x, 2)+ pow(direction.z, 2)), direction.y).Normalize().Dot(Vec2f(1, 0)));
	if(direction.y< 0)
		pitch= -Pitch;

	yaw= -acos(Vec2f(direction.x, direction.z).Normalize().Dot(Vec2f(0, -1)));
	if(direction.x< 0)
		yaw= -Yaw;

	//This is flipped to to support "lower yaw -> turn left"
	yaw= -Yaw;

	roll= 0;

	Touch();
}

void Camera::AssumeOrientation(Camera &other)
{
	position= other.Position;
	pitch= other.Pitch;
	yaw= other.Yaw;
	roll= other.Roll;
	fov= other.FOV;
	orthographic_focus= other.orthographic_focus;

	Touch();
}

void Camera::Translate(Vec3f displacement)
{
	if(is_orthographic)
		orthographic_focus+= displacement;
	else
		position+= displacement;

	Touch();
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
	if(is_orthographic)
		return GetTransform().Merge(Transform().Scale(2.0f/ GetOrthographicHorizontalSize()).Scale(Vec3f(1, 1, -1 /1000.0f)));
	else
		return GetTransform().Merge(Transform::MakeProjectionTransform(fov, aspect_ratio));
}

void Camera::Update()
{
	if(is_orthographic)
	{
		position= GetForward()* -100+ orthographic_focus;
	}
}

Photo Camera::TakePhoto(Scene & scene, Vec2i size, Photo::Type type)
{
	return TakePhotos(scene, size, type)[type];
}
