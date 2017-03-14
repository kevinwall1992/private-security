#ifndef PS_GIZMOS
#define PS_GIZMOS

#include "Gizmo.h"
#include "Camera.h"
#include "Actor.h"


class CameraGizmo : public Gizmo
{
	Camera *camera;

public:
	CameraGizmo(Camera *camera);

	virtual void KeyHold(ButtonType button, float t);

	virtual void MouseDragLeft(Vec2f mouse_position, Vec2f mouse_displacement);
	virtual void MouseDragMiddle(Vec2f mouse_position, Vec2f mouse_displacement);
	virtual void MouseDragRight(Vec2f mouse_position, Vec2f mouse_displacement);
};

#endif