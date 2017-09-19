#ifndef PS_INTERFACE
#define PS_INTERFACE

#include "Drawable.h"
#include "Gizmo.h"
#include "PropertySpecializations.h"
#include "Transform.h"
#include "Framebuffer.h"
#include "Treelike.h"

#include <stack>


class InterfaceContainer;
class WindowContainer;
class Parcel;
class InterfaceEvent;


//Interfaces must be drawn to a framebuffer and viewport with the same size as the screen
//This is to make transforming mouse input unnecessary, which makes everything simpler.
class Interface : public Drawable, public Gizmo, public virtual Treelike<Interface>
{
	static Interface *focused_interface;
	static Parcel *active_parcel;

	vector<Drawable *> drawable_components;
	vector<Gizmo *> gizmo_components;

	vector<Interface *> interface_event_recipients;

	int last_event_id= -1;

	bool is_hovered= false;
	bool is_pressed= false;
	bool is_pressed_without_dragging= false;

	bool activate_on_draw= false;
	bool dont_rebroadcast_current_event= false;
	bool intersect_parent_bounds= true;
	bool is_orphan= false;
	bool accepts_parcels= false;
	bool opaque_to_mouse_input= false;

	vector<Interface *> GetInterfaceComponents();

	void DisposeOfParcel(Parcel *parcel);

	bool IsTopLevelInterface();

protected:
	void ClearInteractionState();

	virtual void AddComponent(Interface *interface_, bool send_to_back= false);
	virtual void AddComponent(Drawable *drawable, bool send_to_back= false);
	virtual void AddComponent(Gizmo *gizmo, bool send_to_back= false);

	virtual void RemoveComponent(Interface *interface_, bool delete_component= false);
	virtual void RemoveComponent(Drawable *drawable, bool delete_component= false);
	virtual void RemoveComponent(Gizmo *gizmo, bool delete_component= false);

	virtual void ReplaceComponent(Interface *interface_, Interface *replacement_interface);
	virtual void ReplaceComponent(Drawable *drawable, Drawable *replacement_drawable);
	virtual void ReplaceComponent(Gizmo *gizmo, Gizmo *replacement_gizmo);

	void MakeOrphan();
	void MakeNotOrphan();

	virtual void RecieveParcel(Parcel *parcel);
	virtual void RelinquishParcel(Parcel *parcel, bool must_transfer_ownership);
	static void SendParcel(Parcel *parcel);
	void AcceptParcels();
	void DontAcceptParcels();
	static bool IsParcelActive();

	virtual void Draw_Components();
	virtual void HandleEvent_Components(Event *event_);

	void SpawnInterfaceEvent(InterfaceEvent *interface_event);

	virtual void HandleMouseEvent(MouseEvent *mouse_event);
	virtual void HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event);
	virtual void HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event);

	virtual void HandleInterfaceEvent(InterfaceEvent *interface_event);

	virtual bool DoesSkip(Event *event_);
	virtual bool DoesConsume(Event *event_);
	void DontRebroadcastCurrentEvent();

	virtual void MouseHover(Vec2f mouse_position);
	virtual void MouseIn();
	virtual void MouseOut();

	virtual void MouseLeftClick();
	virtual void MouseMiddleClick();
	virtual void MouseRightClick();

	virtual void MouseScrollOver(int scroll_amount);
	virtual void MouseLeftDragOver();
	virtual void MouseMiddleDragOver();
	virtual void MouseRightDragOver();
	virtual void Dragged();

	//virtual bool IsInside_Implementation(Vec2f point);

public:
	void AddInterfaceEventRecipient(Interface *other);

	void IntersectParentBounds();
	void DontIntersectParentBounds();
	bool DoesIntersectParentBounds();

	void MakeOpaqueToMouseInput();
	void MakeTransparentToMouseInput();

	bool IsHovered();
	bool IsPressed();
	bool IsFocused();

	virtual bool IsInside(Vec2f point);

	virtual void Activate();
	virtual void Deactivate();

	virtual void Show();
	virtual void Hide();

	virtual void Focus();
	virtual void Defocus();

	virtual void Draw();
	virtual void HandleEvent(Event *event_);

	friend InterfaceContainer;
	friend WindowContainer;
	friend Parcel;
};

//For Events spawned by Interfaces
class InterfaceEvent : public Event
{
	Interface *interface_;

public:
	InterfaceEvent(Interface *component);

	Interface * GetInterface();
	template<class T>
	T * GetInterfaceAs()
	{
		return dynamic_cast<T *>(interface_);
	}

	bool IsComponentChildOf(Interface *interface_);
};

class Parcel : public Drawable, public Gizmo
{
	Interface *sender;

	Parcel(Interface *sender);

protected:
	Interface * GetSender();

public:
	virtual void Draw();

	template<class T>
	class TypedParcel;
};

template<class T>
class Parcel::TypedParcel : public Parcel
{
	T *item;

public:
	TypedParcel(Interface *sender, T *item)
		: Parcel(sender)
	{
		this->item= item;
	}

	bool IsThisTheItem(T *other)
	{
		return item== other;
	}

	T * SignFor(bool must_transfer_ownership= false)
	{
		sender->RelinquishParcel(this, must_transfer_ownership);

		T *item= this->item;
		this->item= nullptr;

		return item;
	}

	template<class U>
	bool IsItemOfType()
	{
		return Utility::IsType<U>(item);
	}
};
class Object;
typedef Parcel::TypedParcel<Object> ObjectParcel;

template<class T>
class WrappedParcel : public Parcel::TypedParcel<T>
{
	Drawable *wrapping;

public:
	WrappedParcel(Interface *sender, T *item, Drawable *wrapping)
		: TypedParcel(sender, item)
	{
		this->wrapping= wrapping;
	}

	virtual void Draw()
	{
		wrapping->Draw();
	}
};

#endif
