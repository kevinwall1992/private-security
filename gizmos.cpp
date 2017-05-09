#include "Gizmos.h"
#include "GameSystem.h"

CameraGizmo::CameraGizmo(Camera *camera_)
{
	camera= camera_;
}

	
void CameraGizmo::KeyHold(ButtonType button, float t)
{
	float pan_speed= 7.5f;
	float fov_speed= 0.1f;

	switch(button)
	{
	case ButtonType::S: 
		if(camera->IsOrthographic())
			camera->FOV*= pow(1+ fov_speed, t);
		else
			camera->Position+= camera->GetForward()* -pan_speed* t; break;

	case ButtonType::W: 
		if(camera->IsOrthographic())
			camera->FOV*= pow(1- fov_speed, t);
		else
			camera->Position+= camera->GetForward()* pan_speed* t; break;

	case ButtonType::A: camera->Position+= camera->GetRight()*	 -pan_speed* t; break;
	case ButtonType::D: camera->Position+= camera->GetRight()*    pan_speed* t; break;
	case ButtonType::Q: camera->Position+= camera->GetUp()*		 -pan_speed* t; break;
	case ButtonType::E: camera->Position+= camera->GetUp()*		  pan_speed* t; break;
	}	
}

void CameraGizmo::MouseDragLeft(Vec2f mouse_position, Vec2f mouse_displacement)
{
	camera->Yaw+= Math::DegreesToRadians(mouse_displacement.x* 200);
	camera->Pitch+= Math::DegreesToRadians(mouse_displacement.y* 200);
}

void CameraGizmo::MouseDragMiddle(Vec2f mouse_position, Vec2f mouse_displacement)
{

}

void CameraGizmo::MouseDragRight(Vec2f mouse_position, Vec2f mouse_displacement)
{

}

