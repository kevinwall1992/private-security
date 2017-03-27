#ifndef PS_GIZMO
#define PS_GIZMO

#include "EventHandler.h"


class Gizmo : public EventHandler
{
	bool is_active= true;

protected:
	typedef ButtonEvent::Button ButtonType;
	typedef ButtonEvent::Type EventType;
	typedef MouseEvent::MouseButton MouseButtonType;

	virtual void HandleKeyboardEvent(ButtonEvent *button_event);

	virtual void KeyUp(ButtonType button);
	virtual void KeyDown(ButtonType button);
	virtual void KeyHold(ButtonType button, float t);

	virtual void HandleMouseEvent(MouseEvent *mouse_event);
	virtual void HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event);
	virtual void HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event);
	
	virtual void MouseLeftUp(Vec2f mouse_position);
	virtual void MouseLeftDown(Vec2f mouse_position);
	virtual void MouseLeftHold(Vec2f mouse_position, float t);
	virtual void MouseLeftDoubleClick(Vec2f mouse_position);

	virtual void MouseMiddleUp(Vec2f mouse_position);
	virtual void MouseMiddleDown(Vec2f mouse_position);
	virtual void MouseMiddleHold(Vec2f mouse_position, float t);
	virtual void MouseMiddleDoubleClick(Vec2f mouse_position);

	virtual void MouseRightUp(Vec2f mouse_position);
	virtual void MouseRightDown(Vec2f mouse_position);
	virtual void MouseRightHold(Vec2f mouse_position, float t);
	virtual void MouseRightDoubleClick(Vec2f mouse_position);

	virtual void MouseScroll(int scroll_amount);

	virtual void MouseMotion(Vec2f mouse_position, Vec2f mouse_displacement);

	virtual void MouseDragLeft(Vec2f mouse_position, Vec2f mouse_displacement);
	virtual void MouseDragMiddle(Vec2f mouse_position, Vec2f mouse_displacement);
	virtual void MouseDragRight(Vec2f mouse_position, Vec2f mouse_displacement);

public:
	virtual void Activate();
	virtual void Deactivate();
	virtual bool IsActive();
	
	virtual void HandleEvent(Event *event_);
};

#endif
