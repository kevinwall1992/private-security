#ifndef PS_INTERFACE
#define PS_INTERFACE

#include "Drawable.h"
#include "Gizmo.h"
#include "PropertySpecializations.h"
#include "Transform.h"
#include "Framebuffer.h"

#include <stack>


class InterfaceContainer;
class WindowContainer;

//Interfaces must be drawn to a framebuffer and viewport with the same size as the screen
//This is to make transforming mouse input unnecessary, which makes everything simpler.

class Interface : public Drawable, public Gizmo
{
	bool is_focused;

protected:
	virtual void HandleEvent(Event *event_);

	virtual void HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event);

	virtual bool DoesSkip(Event *event_);
	virtual bool DoesConsume(Event *event_);

	virtual bool IsInside(Vec2f point)= 0;

	virtual void MouseHover(Vec2f mouse_position);
	virtual void MouseIn();
	virtual void MouseOut();

	void Focus();
	void Defocus();
	bool IsFocused();

public:

	friend InterfaceContainer;
	friend WindowContainer;
};


class InterfaceContainer : public Interface
{
	vector<Interface *> interfaces;

protected:
	virtual void HandleEvent(Event *event_);

	virtual bool IsInside(Vec2f point);

	void SendToFront(Interface *interface_);

	Interface * GetInterface(Vec2f point);

public:
	void AddInterface(Interface *interface_);

	virtual void Draw();
};

class WindowContainer : public InterfaceContainer
{
public:
	void MouseLeftUp(Vec2f mouse_position);
};

#endif
