#ifndef PS_GIZMO
#define PS_GIZMO

#include "EventHandler.h"


class Gizmo : public EventHandler
{

protected:

	typedef ButtonEvent::Button Button;
	typedef ButtonEvent::Type EventType;

	virtual void HandleKeyboardEvent(ButtonEvent *button_event);

	virtual void KeyUp(Button::Enum button, EventType::Enum event_type);
	virtual void KeyDown(Button::Enum button, EventType::Enum event_type);
	virtual void KeyHold(Button::Enum button, EventType::Enum event_type, float t);//Should t be available to all? Should it just be accessed through a TimingSystem

	virtual void HandleMouseEvent(MouseEvent *mouse_event);
	virtual void HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event);
	
	virtual void MouseLeftUp(Vec2i mouse_position);
	virtual void MouseLeftDown(Vec2i mouse_position);
	virtual void MouseLeftHold(Vec2i mouse_position, float t);
	virtual void MouseLeftDoubleClick(Vec2i mouse_position);

	virtual void MouseMiddleUp(Vec2i mouse_position);
	virtual void MouseMiddleDown(Vec2i mouse_position);
	virtual void MouseMiddleHold(Vec2i mouse_position, float t);
	virtual void MouseMiddleDoubleClick(Vec2i mouse_position);

	virtual void MouseRightUp(Vec2i mouse_position);
	virtual void MouseRightDown(Vec2i mouse_position);
	virtual void MouseRightHold(Vec2i mouse_position, float t);
	virtual void MouseRightDoubleClick(Vec2i mouse_position);

	virtual void MouseMove(Vec2i mouse_position, Vec2i mouse_displacement);

	//Need to consider how higher level features like this will not get out of control
	//Also, do we want left, middle, right varieties?
	virtual void MouseDragLeft(Vec2i mouse_position, Vec2i mouse_displacement);
	virtual void MouseDragMiddle(Vec2i mouse_position, Vec2i mouse_displacement);
	virtual void MouseDragRight(Vec2i mouse_position, Vec2i mouse_displacement);

	//Mouse out, mouse in, hover, etc

public:
	
	virtual void HandleEvent(Event *event);
};

class Camera;

class CameraGizmo : public Gizmo
{
	Camera *camera;

public:
	CameraGizmo(Camera *camera);

	//This is not just "key is down at the moment of polling", should be
	//"how long did key spend down between last update and now"
	virtual void KeyHold(Button::Enum button, EventType::Enum event_type, float t);

	virtual void MouseDragLeft(Vec2i mouse_position, Vec2i mouse_displacement);
	virtual void MouseDragMiddle(Vec2i mouse_position, Vec2i mouse_displacement);
	virtual void MouseDragRight(Vec2i mouse_position, Vec2i mouse_displacement);
};

#endif
