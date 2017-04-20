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


Camera::RotationProperty::RotationProperty(float *value_pointer, Camera *camera_, Type type_)
	: Property(value_pointer)
{
	camera= camera_;
	type= type_;
}

Camera::RotationProperty & Camera::RotationProperty::operator=(const RotationProperty &other)
{
	Set(other.Get());
	return *this;
}

void Camera::RotationProperty::Set(const float &value)
{
	if(value== Get())
		return;

	switch (type)
	{
	case Type::Pitch: camera->pitch= value; break;
	case Type::Yaw: camera->yaw= value; break;
	case Type::Roll: camera->roll= value; break;
	default: 
		assert(false && "Camera::RotationProperty::Set(): invalid rotation type");
		break;
	}

	camera->rotation_is_valid= false;
	camera->GeneratePosition();
}


Camera::PositionProperty::PositionProperty(Vec3f *value_pointer, Camera *camera_)
	: Property(value_pointer)
{
	camera= camera_;
}

Camera::PositionProperty & Camera::PositionProperty::operator=(const PositionProperty &other)
{
	Set(other.Get());
	return *this;
}

void Camera::PositionProperty::Set(const Vec3f &value)
{
	if(Get()== value)
		return;

	if(camera->IsOrthographic())
		camera->SetOrthographicFocus(camera->focus+ value- Get());
	else
		camera->position= value;

	camera->position_is_valid= false;
}


Camera::FOVProperty::FOVProperty(float *value_pointer, Camera *camera_)
	: Property(value_pointer)
{
	camera= camera_;
}

Camera::FOVProperty & Camera::FOVProperty::operator=(const FOVProperty &other)
{
	Set(other.Get());
	return *this;
}

void Camera::FOVProperty::Set(const float &value)
{
	camera->fov= value;
}


void Camera::GeneratePosition()
{
	if(!is_orthographic)
		return;

	Vec3f new_position= GetForward()* -100+ focus;
	if(position== new_position)
		return;

	position= new_position;
	position_is_valid= false;
}

void Camera::SetOrthographicFocus(Vec3f focus_)
{
	focus= focus_;
	GeneratePosition();
}

Transform Camera::GetDirectionTransform()
{
	return Transform().RotateAboutZ(Roll).RotateAboutX(Pitch).RotateAboutY(-Yaw);
}

Vec3f Camera::SnapToPixel(Vec3f position, Vec2i pixel_size)
{
	assert(is_orthographic && "Camera::SnapToPixel(): Shouldn't use this in perspective mode");

	Vec3f u= GetRight();
	Vec3f v= GetUp();
	Vec3f f= GetForward();

	Vec3f projected= Vec3f(u.Dot(position), v.Dot(position), f.Dot(position));
	Vec2f pixel_f= (Vec2f)pixel_size* (Vec2f(projected.x, projected.y)/ fov+ Vec2f(1, 1))/ 2;
	Vec2i pixel= pixel_f;
	if(pixel_f.x< 0)
		pixel.x-= 1;
	if(pixel_f.y< 0)
		pixel.y-= 1;

	Vec2f projected_xy= (((Vec2f)pixel/ pixel_size)* 2- Vec2f(1, 1))* fov;
	Vec3f snapped= u* projected_xy.x+ v* projected_xy.y+ f* projected.z;

	return snapped;
}

bool Camera::ValidatePosition()
{
	bool valid= position_is_valid;
	position_is_valid= true;

	return valid;
}

bool Camera::ValidateRotation()
{
	bool valid= rotation_is_valid;
	rotation_is_valid= true;

	return valid;
}


bool Camera::ValidateFOV()
{
	bool valid= fov_is_valid;
	fov_is_valid= true;

	return valid;
}

bool Camera::ValidateAllAttributes()
{
	return ValidatePosition() && ValidateRotation() && ValidateFOV();
}

Camera::Camera(float fov_, Vec3f position_)
	: Position(&position, this), 
	  Pitch(&pitch, this, RotationProperty::Type::Pitch), 
	  Yaw(&yaw, this, RotationProperty::Type::Yaw), 
	  Roll(&roll, this, RotationProperty::Type::Roll), 
	  FOV(&fov, this)
{
	is_orthographic= false;

	FOV= fov_;
	Position= position_;
}

Camera::Camera(float fov_, Vec3f focus, float pitch_, float yaw_)
	: Position(&position, this), 
	  Pitch(&pitch, this, RotationProperty::Type::Pitch), 
	  Yaw(&yaw, this, RotationProperty::Type::Yaw), 
	  Roll(&roll, this, RotationProperty::Type::Roll), 
	  FOV(&fov, this)
{
	is_orthographic= true;

	FOV= fov_;
	Pitch= pitch_;
	Yaw= yaw_;
	SetOrthographicFocus(focus);
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

Vec3f Camera::GetViewPlaneU()
{
	return GetRight()* tan(FOV/ 2);
}

Vec3f Camera::GetViewPlaneV()
{
	return GetUp()* tan(FOV/ 2);
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
	if(is_orthographic)
		SetOrthographicFocus(look_at_position);
	else
	{
		Vec3f direction= (look_at_position- position).Normalized();

		pitch= acos(Vec2f((float)sqrt(pow(direction.x, 2)+ pow(direction.z, 2)), direction.y).Normalize().Dot(Vec2f(1, 0)));
		if(direction.y< 0)
			pitch= -Pitch;

		yaw= -acos(Vec2f(direction.x, direction.z).Normalize().Dot(Vec2f(0, -1)));
		if(direction.x< 0)
			yaw= -Yaw;

		yaw= -Yaw;

		roll= 0;

		rotation_is_valid= false;
	}
}

void Camera::AssumeOrientation(Camera &other)
{
	
	Pitch= other.Pitch;
	Yaw= other.Yaw;
	Roll= other.Roll;
	FOV= other.FOV;
	if(is_orthographic)
	{
		focus= other.focus;
		GeneratePosition();
	}
	else
		Position= other.Position;
}

Transform Camera::GetTransform()
{
	return Transform().RotateAboutX(pitch).RotateAboutY(-yaw).Translate(position).Invert();
}

Transform Camera::GetProjectedTransform(float aspect_ratio)
{
	if(is_orthographic)
		return GetTransform().Merge(Transform().Scale(2.0f/ fov).Scale(Vec3f(1, 1, -1 /1000.0f)));
	else
		return GetTransform().Merge(Transform::MakeProjectionTransform(fov, aspect_ratio));
}

Photo Camera::TakePhoto(Scene & scene, Vec2i size, Photo::Type type)
{
	return TakePhotos(scene, size, type)[type];
}
