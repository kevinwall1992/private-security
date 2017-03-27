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
	vector<Drawable *> drawable_components;
	vector<Gizmo *> gizmo_components;

	bool is_hovered= false;
	bool is_pressed= false;
	bool activate_on_draw= false;

protected:
	void ClearInteractionState();

	void AddComponent(Interface *interface_);
	void AddComponent(Drawable *drawable);
	void AddComponent(Gizmo *gizmo);

	void RemoveComponent(Interface *interface_);
	void RemoveComponent(Drawable *drawable);
	void RemoveComponent(Gizmo *gizmo);

	void ReplaceComponent(Interface *interface_, Interface *replacement_interface);
	void ReplaceComponent(Drawable *drawable, Drawable *replacement_drawable);
	void ReplaceComponent(Gizmo *gizmo, Gizmo *replacement_gizmo);

	virtual void Draw_Components();
	virtual void HandleEvent_Components(Event *event_);

	virtual void HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event);
	virtual void HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event);

	virtual bool DoesSkip(Event *event_);
	virtual bool DoesConsume(Event *event_);

	virtual bool IsInside(Vec2f point)= 0;

	virtual void MouseHover(Vec2f mouse_position);
	virtual void MouseIn();
	virtual void MouseOut();

	virtual void MouseLeftClick();
	virtual void MouseMiddleClick();
	virtual void MouseRightClick();

	bool IsHovered();
	bool IsPressed();

public:
	virtual void Activate();
	virtual void Deactivate();

	virtual void Show();
	virtual void Hide();

	virtual void Draw();
	virtual void HandleEvent(Event *event_);

	friend InterfaceContainer;
	friend WindowContainer;
};

#endif
