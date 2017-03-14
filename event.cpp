#include "Event.h"

Event::Event(float elapsed_seconds_)
{
	elapsed_seconds= elapsed_seconds_;
}

Event::~Event()
{

}

float Event::GetElapsedSeconds()
{
	return elapsed_seconds;
}

ButtonEvent::ButtonEvent(float elapsed_seconds, Button button_, Type type_)
	: Event(elapsed_seconds)
{
	button= button_;
	type= type_;
}

ButtonEvent::Button ButtonEvent::GetButton()
{
	return button;
}

ButtonEvent::Type ButtonEvent::GetType()
{
	return type;
}


MouseEvent::MouseEvent(float elapsed_seconds, MouseButton button_, Vec2f mouse_position_)
	: Event(elapsed_seconds)
{
	button= button_;
	mouse_position= mouse_position_;
}

MouseEvent::MouseButton MouseEvent::GetMouseButton()
{
	return button;
}

Vec2f MouseEvent::GetMousePosition()
{
	return mouse_position;
}

MouseButtonEvent::MouseButtonEvent(float elapsed_seconds, MouseButton button, Type type, Vec2f mouse_position)
	: Event(elapsed_seconds), ButtonEvent(elapsed_seconds, (Button)button, type), MouseEvent(elapsed_seconds, button, mouse_position)
{

}

MouseMotionEvent::MouseMotionEvent(float elapsed_seconds, Vec2f mouse_position, Vec2f mouse_displacement_)
	: Event(elapsed_seconds), MouseEvent(elapsed_seconds, MouseButton::None, mouse_position)
{
	mouse_displacement= mouse_displacement_;
}

Vec2f MouseMotionEvent::GetMouseDisplacement()
{
	return mouse_displacement;
}

MouseDragEvent::MouseDragEvent(float elapsed_seconds, Vec2f mouse_position, Vec2f mouse_displacement, MouseButton button_)
	: Event(elapsed_seconds), MouseMotionEvent(elapsed_seconds, mouse_position, mouse_displacement)
{
	button= button_;
}


MouseScrollEvent::MouseScrollEvent(float elapsed_seconds, Vec2f mouse_position, int scroll_amount_)
	: Event(elapsed_seconds), MouseEvent(elapsed_seconds, MouseButton::Middle, mouse_position)
{
	scroll_amount= scroll_amount_;
}

int MouseScrollEvent::GetScrollAmount()
{
	return scroll_amount;
}
