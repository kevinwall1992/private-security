#include "InputSystem.h"
#include "Gizmo.h"

#include <SDL.h>


ButtonEvent::Button::Enum InputSystem::SDLKToButton(int sdlk_value)
{
	switch(sdlk_value)
	{
	case SDLK_a: return ButtonEvent::Button::A;
	case SDLK_b: return ButtonEvent::Button::B;
	case SDLK_c: return ButtonEvent::Button::C;
	case SDLK_d: return ButtonEvent::Button::D;
	case SDLK_e: return ButtonEvent::Button::E;
	case SDLK_f: return ButtonEvent::Button::F;
	case SDLK_g: return ButtonEvent::Button::G;
	case SDLK_h: return ButtonEvent::Button::H;
	case SDLK_i: return ButtonEvent::Button::I;
	case SDLK_j: return ButtonEvent::Button::J;
	case SDLK_k: return ButtonEvent::Button::K;
	case SDLK_l: return ButtonEvent::Button::L;
	case SDLK_m: return ButtonEvent::Button::M;
	case SDLK_n: return ButtonEvent::Button::N;
	case SDLK_o: return ButtonEvent::Button::O;
	case SDLK_p: return ButtonEvent::Button::P;
	case SDLK_q: return ButtonEvent::Button::Q;
	case SDLK_r: return ButtonEvent::Button::R;
	case SDLK_s: return ButtonEvent::Button::S;
	case SDLK_t: return ButtonEvent::Button::T;
	case SDLK_u: return ButtonEvent::Button::U;
	case SDLK_v: return ButtonEvent::Button::V;
	case SDLK_w: return ButtonEvent::Button::W;
	case SDLK_x: return ButtonEvent::Button::X;
	case SDLK_y: return ButtonEvent::Button::Y;
	case SDLK_z: return ButtonEvent::Button::Z;

	case SDLK_LSHIFT: 
	case SDLK_RSHIFT: return ButtonEvent::Button::Shift;
	case SDLK_LALT: 
	case SDLK_RALT: return ButtonEvent::Button::Alt;
	case SDLK_LCTRL: 
	case SDLK_RCTRL: return ButtonEvent::Button::Ctrl;

	case SDLK_ESCAPE: return ButtonEvent::Button::Esc;
	case SDLK_SPACE: return ButtonEvent::Button::Space;
	case SDLK_DELETE: return ButtonEvent::Button::Del;

	default: return ButtonEvent::Button::None;
	}
}

ButtonEvent::Button::Enum InputSystem::SDL_ScancodeToButton(int sdl_scancode)
{
	switch(sdl_scancode)
	{
	case SDL_SCANCODE_A: return ButtonEvent::Button::A;
	case SDL_SCANCODE_B: return ButtonEvent::Button::B;
	case SDL_SCANCODE_C: return ButtonEvent::Button::C;
	case SDL_SCANCODE_D: return ButtonEvent::Button::D;
	case SDL_SCANCODE_E: return ButtonEvent::Button::E;
	case SDL_SCANCODE_F: return ButtonEvent::Button::F;
	case SDL_SCANCODE_G: return ButtonEvent::Button::G;
	case SDL_SCANCODE_H: return ButtonEvent::Button::H;
	case SDL_SCANCODE_I: return ButtonEvent::Button::I;
	case SDL_SCANCODE_J: return ButtonEvent::Button::J;
	case SDL_SCANCODE_K: return ButtonEvent::Button::K;
	case SDL_SCANCODE_L: return ButtonEvent::Button::L;
	case SDL_SCANCODE_M: return ButtonEvent::Button::M;
	case SDL_SCANCODE_N: return ButtonEvent::Button::N;
	case SDL_SCANCODE_O: return ButtonEvent::Button::O;
	case SDL_SCANCODE_P: return ButtonEvent::Button::P;
	case SDL_SCANCODE_Q: return ButtonEvent::Button::Q;
	case SDL_SCANCODE_R: return ButtonEvent::Button::R;
	case SDL_SCANCODE_S: return ButtonEvent::Button::S;
	case SDL_SCANCODE_T: return ButtonEvent::Button::T;
	case SDL_SCANCODE_U: return ButtonEvent::Button::U;
	case SDL_SCANCODE_V: return ButtonEvent::Button::V;
	case SDL_SCANCODE_W: return ButtonEvent::Button::W;
	case SDL_SCANCODE_X: return ButtonEvent::Button::X;
	case SDL_SCANCODE_Y: return ButtonEvent::Button::Y;
	case SDL_SCANCODE_Z: return ButtonEvent::Button::Z;

	case SDL_SCANCODE_LSHIFT: 
	case SDL_SCANCODE_RSHIFT: return ButtonEvent::Button::Shift;
	case SDL_SCANCODE_LALT: 
	case SDL_SCANCODE_RALT: return ButtonEvent::Button::Alt;
	case SDL_SCANCODE_LCTRL: 
	case SDL_SCANCODE_RCTRL: return ButtonEvent::Button::Ctrl;

	case SDL_SCANCODE_ESCAPE: return ButtonEvent::Button::Esc;
	case SDL_SCANCODE_SPACE: return ButtonEvent::Button::Space;
	case SDL_SCANCODE_DELETE: return ButtonEvent::Button::Del;

	default: return ButtonEvent::Button::None;
	}
}

MouseEvent::MouseButton::Enum InputSystem::SDLMouseButtonToButton(int sdl_mouse_button_value)
{
	switch(sdl_mouse_button_value)
	{
	case SDL_BUTTON_LEFT: return MouseEvent::MouseButton::MouseLeft;
	case SDL_BUTTON_MIDDLE: return MouseEvent::MouseButton::MouseMiddle;
	case SDL_BUTTON_RIGHT: return MouseEvent::MouseButton::MouseRight;
	default: return MouseEvent::MouseButton::None;
	}
}

vector<MouseEvent::MouseButton::Enum> InputSystem::GetDownMouseButtons(unsigned int state)
{
	vector<MouseEvent::MouseButton::Enum> down_mouse_buttons;

	for(int i= SDL_BUTTON_LEFT; i< SDL_BUTTON_RIGHT; i++)
		if(state& SDL_BUTTON(i))
		{
			MouseEvent::MouseButton::Enum button= SDLMouseButtonToButton(i);
			if(button!= MouseEvent::MouseButton::None)
				down_mouse_buttons.push_back(button);
		}

	return down_mouse_buttons;
}

void InputSystem::BroadcastEvent(Event *event)
{
	for(unsigned int i= 0; i< gizmos.size(); i++)
		gizmos[i]->HandleEvent(event);

	delete event;
}

void InputSystem::AddGizmo(Gizmo *gizmo)
{
	gizmos.push_back(gizmo);
}

bool InputSystem::HandleInput()
{
	float elapsed_seconds= timer->Stop();
	timer->Start();

	SDL_PumpEvents();

	Vec2i mouse_position;
	Uint32 mouse_state= SDL_GetMouseState(&mouse_position.c[0], &mouse_position.c[1]);
	Vec2i mouse_displacement= last_mouse_position- mouse_position;

	SDL_Event sdl_event;
	while(SDL_PollEvent(&sdl_event))
	{
		if(sdl_event.type== SDL_QUIT)
			return false;
		
		vector<MouseEvent::MouseButton::Enum> down_mouse_buttons;

		switch(sdl_event.type)
		{
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			BroadcastEvent(new MouseButtonEvent(elapsed_seconds, 
												SDLMouseButtonToButton(sdl_event.button.button), 
												sdl_event.button.type== SDL_MOUSEBUTTONUP ? MouseButtonEvent::Type::Up : MouseButtonEvent::Type::Down, 
												mouse_position));
			break;

		case SDL_MOUSEMOTION:
			BroadcastEvent(new MouseMotionEvent(elapsed_seconds, 
												mouse_position, 
												mouse_displacement));

			down_mouse_buttons= GetDownMouseButtons(sdl_event.motion.state);
			for(unsigned int i= 0; i< down_mouse_buttons.size(); i++)
				BroadcastEvent(new MouseDragEvent(elapsed_seconds, 
												  mouse_position,
												  mouse_displacement,
												  down_mouse_buttons[i]));
			break;

		case SDL_KEYUP:
		case SDL_KEYDOWN:
			BroadcastEvent(new ButtonEvent(elapsed_seconds, 
										   SDLKToButton(sdl_event.key.keysym.sym), 
										   sdl_event.key.type== SDL_KEYUP ? ButtonEvent::Type::Up : ButtonEvent::Type::Down));
			break;

		default: break;
		}
	}


	vector<MouseEvent::MouseButton::Enum> down_mouse_buttons= GetDownMouseButtons(mouse_state);
	for(unsigned int i= 0; i< down_mouse_buttons.size(); i++)
		BroadcastEvent(new MouseButtonEvent(elapsed_seconds, 
											down_mouse_buttons[i], 
											MouseButtonEvent::Type::Hold, 
											mouse_position));

	int key_count;
	const Uint8 *keys= SDL_GetKeyboardState(&key_count);

	for(int i= 0; i< key_count; i++)
	{
		if(keys[i])
		{
			ButtonEvent::Button::Enum button= SDL_ScancodeToButton(i);
			if(button!= ButtonEvent::Button::None)
				BroadcastEvent(new ButtonEvent(elapsed_seconds, 
					                           button, 
					                           ButtonEvent::Type::Hold));
		}
	}
	

	last_mouse_position= mouse_position;

	return true;
}

void InputSystem::Initialize()
{
	SDL_Init(SDL_INIT_EVENTS);

	timer= new Timer();
}

void InputSystem::Terminate()
{
	delete timer;

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
}
