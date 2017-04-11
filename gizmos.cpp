#include "Gizmos.h"
#include "GameSystem.h"

CameraGizmo::CameraGizmo(Camera *camera_)
{
	camera= camera_;
}

	
void CameraGizmo::KeyHold(ButtonType button, float t)
{
	float speed= 2.5f;

	switch(button)
	{
	case ButtonType::S: camera->Translate(camera->GetForward()* -speed* t); break;
	case ButtonType::W: camera->Translate(camera->GetForward()*  speed* t); break;
	case ButtonType::A: camera->Translate(camera->GetRight()*	-speed* t); break;
	case ButtonType::D: camera->Translate(camera->GetRight()*    speed* t); break;
	case ButtonType::Q: camera->Translate(camera->GetUp()*		-speed* t); break;
	case ButtonType::E: camera->Translate(camera->GetUp()*		 speed* t); break;
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

