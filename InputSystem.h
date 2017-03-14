#ifndef PS_INPUT_SYSTEM
#define PS_INPUT_SYSTEM

#include "System.h"
#include "Gizmo.h"
#include "Timer.h"


class InputSystem : public System
{
	vector<Gizmo *> gizmos;

	Timer *timer;
	Vec2i last_mouse_position;

	Vec2f GetNormalizedMousePosition(Vec2i mouse_position);

	static ButtonEvent::Button SDLKToButton(int sdlk_value);
	static ButtonEvent::Button SDL_ScancodeToButton(int sdl_scancode);
	static MouseEvent::MouseButton SDLMouseButtonToButton(int sdl_mouse_button_value);
	static vector<MouseEvent::MouseButton> GetDownMouseButtons(unsigned int state);

	void BroadcastEvent(Event *event);

public:
	void AddGizmo(Gizmo *gizmo);

	bool HandleInput();

	void Initialize();
	void Terminate();
};

#endif
