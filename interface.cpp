#include "Interface.h"
#include "GarbageCan.h"
#include "Dropdown.h"
#include "Menu.h"
#include "UI.h"


Interface * Interface::focused_interface= nullptr;
Parcel * Interface::active_parcel= nullptr;

vector<Interface*> Interface::GetInterfaceComponents()
{
	vector<Interface*> interface_components;

	for(Drawable *drawable : drawable_components)
		if(Utility::IsType<Interface>(drawable))
			interface_components.push_back(dynamic_cast<Interface *>(drawable));

	return interface_components;
}

void Interface::DisposeOfParcel(Parcel *parcel)
{
	if(parcel== active_parcel)
		active_parcel= nullptr;

	GarbageCan<Parcel>::ThrowAway(parcel);
}

bool Interface::IsTopLevelInterface()
{
	return GetParent()== nullptr && !is_orphan;
}

void Interface::ClearInteractionState()
{
	is_hovered= false;
	is_pressed= false;

	for(Gizmo *gizmo : gizmo_components)
	{
		Interface *interface_= dynamic_cast<Interface *>(gizmo);

		if(interface_!= nullptr)
			interface_->ClearInteractionState();
	}
}

void Interface::AddComponent(Interface *interface_, bool send_to_back)
{
	AddComponent((Drawable *)interface_, send_to_back);
	AddComponent((Gizmo *)interface_, send_to_back);

	interface_->SetParent(this);
}

void Interface::AddComponent(Drawable *drawable, bool send_to_back)
{
	if(send_to_back)
		drawable_components.insert(drawable_components.begin(), drawable);
	else
		drawable_components.push_back(drawable);
}

void Interface::AddComponent(Gizmo *gizmo, bool send_to_back)
{
	if(send_to_back)
		gizmo_components.push_back(gizmo);
	else
		gizmo_components.insert(gizmo_components.begin(), gizmo);
}

void Interface::RemoveComponent(Interface *interface_, bool delete_component)
{
	Utility::RemoveElement<Drawable *>(drawable_components, interface_);
	Utility::RemoveElement<Gizmo *>(gizmo_components, interface_);

	if(delete_component)
		GarbageCan<Interface>::ThrowAway(interface_);
}

void Interface::RemoveComponent(Drawable *drawable, bool delete_component)
{
	Utility::RemoveElement(drawable_components, drawable);

	if(delete_component)
		GarbageCan<Drawable>::ThrowAway(drawable);
}

void Interface::RemoveComponent(Gizmo *gizmo, bool delete_component)
{
	Utility::RemoveElement(gizmo_components, gizmo);

	if(delete_component)
		GarbageCan<Gizmo>::ThrowAway(gizmo);
}

void Interface::ReplaceComponent(Interface *interface_, Interface *replacement_interface)
{
	Utility::ReplaceElement<Drawable *>(drawable_components, interface_, replacement_interface);
	Utility::ReplaceElement<Gizmo *>(gizmo_components, interface_, replacement_interface);
}

void Interface::ReplaceComponent(Drawable *drawable, Drawable *replacement_drawable)
{
	Utility::ReplaceElement(drawable_components, drawable, replacement_drawable);
}

void Interface::ReplaceComponent(Gizmo *gizmo, Gizmo *replacement_gizmo)
{
	Utility::ReplaceElement(gizmo_components, gizmo, replacement_gizmo);
}

void Interface::MakeOrphan()
{
	is_orphan= true;
}

void Interface::Draw_Components()
{
	if(IsHidden())
		return;

	for(Drawable *drawable : drawable_components)
		if(!(Utility::IsType<Interface>(drawable) && dynamic_cast<Interface *>(drawable)->IsFocused()))
			drawable->Draw();

	if(IsTopLevelInterface())
	{
		if(focused_interface!= nullptr)
			focused_interface->Draw();

		if(active_parcel!= nullptr)
			active_parcel->Draw();

		Menu::InitializeMenus();
		Menu::Draw_Menus();
	}
}

void Interface::HandleEvent_Components(Event *event_)
{
	if(!IsActive())
		return;

	vector<Gizmo *> gizmo_components_copy= gizmo_components;
	gizmo_components_copy.erase(std::remove(gizmo_components_copy.begin(), 
		                                    gizmo_components_copy.end(), 
		                                    focused_interface), 
		                        gizmo_components_copy.end());
	if(GetParent()== nullptr && !is_orphan && focused_interface!= nullptr)
		gizmo_components_copy.insert(gizmo_components_copy.begin(), focused_interface);

	bool event_consumed= false;
	for(Gizmo *gizmo : gizmo_components_copy)
	{
		if(!gizmo->IsActive())
			continue;

		if(Utility::IsType<Interface>(gizmo))
		{
			if(!event_consumed)
			{
				if(dynamic_cast<Interface *>(gizmo)->DoesConsume(event_))
					event_consumed= true;

				gizmo->HandleEvent(event_);
			}
		}
		else
			gizmo->HandleEvent(event_);
	}

	if(IsTopLevelInterface())
	{
		if(active_parcel!= nullptr)
			active_parcel->HandleEvent(event_);
	}
}

void Interface::SpawnInterfaceEvent(InterfaceEvent *interface_event)
{
	for(Interface *interface_ : interface_event_recipients)
		interface_->HandleEvent(interface_event);

	if(GetParent()!= nullptr)
		GetParent()->HandleEvent(interface_event);
}

void Interface::HandleMouseEvent(MouseEvent *mouse_event)
{
	if(Utility::IsType<MouseScrollEvent>(mouse_event) && IsInside(mouse_event->GetMousePosition()))
		MouseScrollOver(dynamic_cast<MouseScrollEvent *>(mouse_event)->GetScrollAmount());

	Gizmo::HandleMouseEvent(mouse_event);
}

void Interface::HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event)
{
	if(IsInside(mouse_button_event->GetMousePosition()))
	{
		switch(mouse_button_event->GetType())
		{
		case EventType::Down:
			if(mouse_button_event->GetButton()== MouseButtonType::Left)
			{
				is_pressed= true;
				is_pressed_without_dragging= true;
			}
			break;

		case EventType::Up:
			switch(mouse_button_event->GetButton())
			{
			case MouseButtonType::Left: 
				is_pressed= false;
				is_pressed_without_dragging= false;

				if(active_parcel!= nullptr && accepts_parcels)
				{
					this->RecieveParcel(active_parcel);
					DisposeOfParcel(active_parcel);
				}
				else
					MouseLeftClick(); 
				break;
			case MouseButtonType::Middle: MouseMiddleClick(); break;
			case MouseButtonType::Right: MouseRightClick(); break;
			}
			break;
		}
	}

	Gizmo::HandleMouseButtonEvent(mouse_button_event);
}

void Interface::HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event)
{
	Vec2f mouse_position= mouse_motion_event->GetMousePosition();
	Vec2f previous_mouse_position= mouse_position- mouse_motion_event->GetMouseDisplacement();

	if(IsInside(mouse_position))
	{
		if(!IsInside(previous_mouse_position))
			MouseIn();

		MouseHover(mouse_position);
		is_hovered= true;

		if(is_pressed_without_dragging)
		{
			Dragged();
			is_pressed_without_dragging= false;
		}
	}
	else if(IsInside(previous_mouse_position))
	{
		MouseOut();
		is_hovered= false;
	}

	if(Utility::IsType<MouseDragEvent>(mouse_motion_event))
	{
		MouseDragEvent *mouse_drag_event= dynamic_cast<MouseDragEvent *>(mouse_motion_event);

		switch(mouse_drag_event->GetMouseButton())
		{
			case MouseEvent::MouseButton::Left:
				MouseLeftDragOver();
			case MouseEvent::MouseButton::Middle:
				MouseMiddleDragOver();
			case MouseEvent::MouseButton::Right:
				MouseRightDragOver();
		}
	}

	Gizmo::HandleMouseMotionEvent(mouse_motion_event);
}

void Interface::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	for(Interface *interface_ : interface_event_recipients)
		interface_->HandleEvent(interface_event);

	if(GetParent()!= nullptr)
		GetParent()->HandleEvent(interface_event);
}

bool Interface::DoesSkip(Event *event_)
{
	return last_event_id== event_->GetEventID();
}

bool Interface::DoesConsume(Event *event_)
{
	if(opaque_to_mouse_input && ((Utility::IsType<MouseButtonEvent>(event_) || Utility::IsType<MouseScrollEvent>(event_)) && IsInside(dynamic_cast<MouseEvent *>(event_)->GetMousePosition())))
		return true;
	
	for(Gizmo *gizmo : gizmo_components)
		if(Utility::IsType<Interface>(gizmo))
			if(dynamic_cast<Interface *>(gizmo)->DoesConsume(event_))
				return true;

	return false;
}

void Interface::DontRebroadcastCurrentEvent()
{
	dont_rebroadcast_current_event= true;
}

void Interface::MouseHover(Vec2f mouse_position) { }
void Interface::MouseIn() { }
void Interface::MouseOut() { }

void Interface::MouseLeftClick()
{
}

void Interface::MouseMiddleClick()
{
}

void Interface::MouseRightClick()
{
	if(active_parcel!= nullptr)
		DisposeOfParcel(active_parcel);
}

void Interface::MouseScrollOver(int scroll_amount)
{
}

void Interface::MouseLeftDragOver()
{
}

void Interface::MouseMiddleDragOver()
{
}

void Interface::MouseRightDragOver()
{
}

void Interface::Dragged()
{
}

void Interface::AddInterfaceEventRecipient(Interface *other)
{
	interface_event_recipients.push_back(other);
}

void Interface::IntersectParentBounds()
{
	intersect_parent_bounds= true;
}

void Interface::DontIntersectParentBounds()
{
	intersect_parent_bounds= false;
}

bool Interface::DoesIntersectParentBounds()
{
	return intersect_parent_bounds;
}

void Interface::MakeOpaqueToMouseInput()
{
	opaque_to_mouse_input= true;
}

void Interface::MakeTransparentToMouseInput()
{
	opaque_to_mouse_input= false;
}

bool Interface::IsHovered()
{
	return is_hovered;
}

bool Interface::IsPressed()
{
	return is_pressed;
}

bool Interface::IsFocused()
{
	return focused_interface== this;
}

bool Interface::IsInside(Vec2f point)
{
	return true;
}

void Interface::RecieveParcel(Parcel *parcel)
{
	
}

void Interface::RelinquishParcel(Parcel *parcel, bool must_transfer_ownership)
{
}

void Interface::SendParcel(Parcel *parcel)
{
	active_parcel= parcel;
}

void Interface::AcceptParcels()
{
	accepts_parcels= true;
}

bool Interface::IsParcelActive()
{
	return active_parcel!= nullptr;
}

void Interface::Activate()
{
	Gizmo::Activate();
}

void Interface::Deactivate()
{
	Gizmo::Deactivate();

	ClearInteractionState();
}

void Interface::Show()
{
	Drawable::Show();

	activate_on_draw= true;
}

void Interface::Hide()
{
	Drawable::Hide();

	Deactivate();
}

void Interface::Focus()
{
	if(IsFocused())
		return;

	if(focused_interface!= nullptr)
		focused_interface->Defocus();

	focused_interface= this;
}

void Interface::Defocus()
{
	if(focused_interface== this)
		focused_interface= nullptr;
}

void Interface::Draw()
{
	if(IsHidden())
		return;

	Draw_Components();

	if(activate_on_draw)
	{
		Activate();
		activate_on_draw= false;
	}
}

void Interface::HandleEvent(Event *event_)
{
	bool deactivated_by_menu= IsTopLevelInterface() && Menu::IsAMenuOpen();
	if((!IsActive() || deactivated_by_menu) && !Utility::IsType<InterfaceEvent>(event_))
	{
		if(deactivated_by_menu)
			Menu::HandleEvent_Menus(event_);

		return;
	}

	if(!DoesSkip(event_))
	{
		last_event_id= event_->GetEventID();

		if(Utility::IsType<InterfaceEvent>(event_))
		{
			HandleInterfaceEvent(dynamic_cast<InterfaceEvent *>(event_));
		}
		else
		{
			Gizmo::HandleEvent(event_);

			if(dont_rebroadcast_current_event)
				dont_rebroadcast_current_event= false;
			else
				HandleEvent_Components(event_);
		}
	}
}


InterfaceEvent::InterfaceEvent(Interface *interface_)
	: Event(0.0f)
{
	this->interface_= interface_;
}

Interface * InterfaceEvent::GetInterface()
{
	return interface_;
}

bool InterfaceEvent::IsComponentChildOf(Interface *interface_)
{
	return this->interface_->GetParent()== interface_;
}


Interface * Parcel::GetSender()
{
	return sender;
}

Parcel::Parcel(Interface *sender_)
{
	sender= sender_;
}

void Parcel::Draw()
{
}
