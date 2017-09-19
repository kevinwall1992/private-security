#ifndef PS_EVENT
#define PS_EVENT

#include "Vector.h"
#include "Common.h"


class Event : public Polymorphic
{
	static int next_event_id;

	int event_id;

	float elapsed_seconds;

public:
	Event(float elapsed_seconds);
	virtual ~Event();

	int GetEventID();

	float GetElapsedSeconds();
};

class ButtonEvent : public virtual Event
{
public:
	enum Button { None, 
				  MouseLeft, MouseMiddle, MouseRight, 
				  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, 
				  CapitalA, CapitalB, CapitalC, CapitalD, CapitalE, CapitalF, CapitalG, CapitalH, 
		          CapitalI, CapitalJ, CapitalK, CapitalL, CapitalM, CapitalN, CapitalO, CapitalP, 
		          CapitalQ, CapitalR, CapitalS, CapitalT, CapitalU, CapitalV, CapitalW, CapitalX, 
		          CapitalY, CapitalZ,
				  ExclaimationMark, At, Pound, Dollar, Percent, Caret, And, Asterisk, LeftParenthesis, RightParenthesis, 
		          Minus, Underscore, Equals, Plus, Apostrophe, QuotationMark, Comma, Period, ForwardSlash, QuestionMark,
				  One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Zero,
				  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, 
				  Shift, Ctrl, Alt, Tab, Esc, Space, Del, Backspace };

	enum Type { Down= 1, Up, Hold, DoubleClick };

private:
	Button button;
	Type type;

public:
	ButtonEvent(float elapsed_seconds, Button button, Type type);

	Button GetButton();
	Type GetType();
};

class MouseEvent : public virtual Event
{
public:
	enum MouseButton { None, Left, Middle, Right };

protected:
	MouseButton button;
	Vec2f mouse_position;

public:
	MouseEvent(float elapsed_seconds, MouseButton button, Vec2f mouse_position);

	MouseButton GetMouseButton();
	Vec2f GetMousePosition();
};

class MouseScrollEvent : public MouseEvent
{
	int scroll_amount;

public:
	MouseScrollEvent(float elapsed_seconds, Vec2f mouse_position, int scroll_amount);

	int GetScrollAmount();
};

class MouseButtonEvent : public ButtonEvent, public MouseEvent
{
public:
	MouseButtonEvent(float elapsed_seconds, MouseButton button, Type type, Vec2f mouse_position);
};

class MouseMotionEvent : public MouseEvent
{
protected:
	Vec2f mouse_displacement;

public:
	MouseMotionEvent(float elapsed_seconds, Vec2f mouse_position, Vec2f mouse_displacement);

	Vec2f GetMouseDisplacement();
};

class MouseDragEvent : public MouseMotionEvent
{
public:
	MouseDragEvent(float elapsed_seconds, Vec2f mouse_position, Vec2f mouse_displacement, MouseButton button);
};

#endif
