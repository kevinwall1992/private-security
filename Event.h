#ifndef PS_EVENT
#define PS_EVENT

#include "Vector.h"
#include "Common.h"

//Not sure I like elapsed seconds being a member variable of Event...
class Event : public Polymorphic
{
	float elapsed_seconds;

public:
	Event(float elapsed_seconds);
	virtual ~Event();

	float GetElapsedSeconds();
};

class ButtonEvent : public virtual Event
{
public:
	struct Button { enum Enum { None, 
								MouseLeft, MouseMiddle, MouseRight, 
								A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
								Shift, Ctrl, Alt, Tab, Esc, Space, Del}; };

	struct Type { enum Enum { None, Down, Up, Hold, DoubleClick }; };

private:
	Button::Enum button;
	Type::Enum type;

public:
	ButtonEvent(float elapsed_seconds, Button::Enum button, Type::Enum type);

	Button::Enum GetButton();
	Type::Enum GetType();
};

class MouseEvent : public virtual Event
{
public:
	struct MouseButton { enum Enum { None, MouseLeft, MouseMiddle, MouseRight }; };

protected:
	MouseButton::Enum button;
	Vec2i mouse_position;

public:
	MouseEvent(float elapsed_seconds, MouseButton::Enum button, Vec2i mouse_position);

	MouseButton::Enum GetMouseButton();
	Vec2i GetMousePosition();
};

class MouseButtonEvent : public ButtonEvent, public MouseEvent
{
public:
	MouseButtonEvent(float elapsed_seconds, MouseButton::Enum button, Type::Enum type, Vec2i mouse_position);
};

class MouseMotionEvent : public MouseEvent
{
protected:
	Vec2i mouse_displacement;

public:
	MouseMotionEvent(float elapsed_seconds, Vec2i mouse_position, Vec2i mouse_displacement);

	Vec2i GetMouseDisplacement();
};

class MouseDragEvent : public MouseMotionEvent
{
public:
	MouseDragEvent(float elapsed_seconds, Vec2i mouse_position, Vec2i mouse_displacement, MouseButton::Enum button);
};

#endif
