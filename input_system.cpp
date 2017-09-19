#include "InputSystem.h"
#include "GraphicsSystem.h"
#include "Gizmo.h"

#include <SDL.h>


Vec2f InputSystem::GetNormalizedMousePosition(Vec2i mouse_position)
{
	mouse_position.y= System::graphics.GetScreenSize().y- mouse_position.y;

	return ((Vec2f)mouse_position)/ (float)System::graphics.GetScreenSize().x;
}

ButtonEvent::Button InputSystem::SDLKToButton(int sdlk_value)
{
	const Uint8 *keys= SDL_GetKeyboardState(nullptr);
	bool is_shift_down= keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

	switch(sdlk_value)
	{
	case SDLK_a: return !is_shift_down ? ButtonEvent::Button::A : ButtonEvent::Button::CapitalA;
	case SDLK_b: return !is_shift_down ? ButtonEvent::Button::B : ButtonEvent::Button::CapitalB;
	case SDLK_c: return !is_shift_down ? ButtonEvent::Button::C : ButtonEvent::Button::CapitalC;
	case SDLK_d: return !is_shift_down ? ButtonEvent::Button::D : ButtonEvent::Button::CapitalD;
	case SDLK_e: return !is_shift_down ? ButtonEvent::Button::E : ButtonEvent::Button::CapitalE;
	case SDLK_f: return !is_shift_down ? ButtonEvent::Button::F : ButtonEvent::Button::CapitalF;
	case SDLK_g: return !is_shift_down ? ButtonEvent::Button::G : ButtonEvent::Button::CapitalG;
	case SDLK_h: return !is_shift_down ? ButtonEvent::Button::H : ButtonEvent::Button::CapitalH;
	case SDLK_i: return !is_shift_down ? ButtonEvent::Button::I : ButtonEvent::Button::CapitalI;
	case SDLK_j: return !is_shift_down ? ButtonEvent::Button::J : ButtonEvent::Button::CapitalJ;
	case SDLK_k: return !is_shift_down ? ButtonEvent::Button::K : ButtonEvent::Button::CapitalK;
	case SDLK_l: return !is_shift_down ? ButtonEvent::Button::L : ButtonEvent::Button::CapitalL;
	case SDLK_m: return !is_shift_down ? ButtonEvent::Button::M : ButtonEvent::Button::CapitalM;
	case SDLK_n: return !is_shift_down ? ButtonEvent::Button::N : ButtonEvent::Button::CapitalN;
	case SDLK_o: return !is_shift_down ? ButtonEvent::Button::O : ButtonEvent::Button::CapitalO;
	case SDLK_p: return !is_shift_down ? ButtonEvent::Button::P : ButtonEvent::Button::CapitalP;
	case SDLK_q: return !is_shift_down ? ButtonEvent::Button::Q : ButtonEvent::Button::CapitalQ;
	case SDLK_r: return !is_shift_down ? ButtonEvent::Button::R : ButtonEvent::Button::CapitalR;
	case SDLK_s: return !is_shift_down ? ButtonEvent::Button::S : ButtonEvent::Button::CapitalS;
	case SDLK_t: return !is_shift_down ? ButtonEvent::Button::T : ButtonEvent::Button::CapitalT;
	case SDLK_u: return !is_shift_down ? ButtonEvent::Button::U : ButtonEvent::Button::CapitalU;
	case SDLK_v: return !is_shift_down ? ButtonEvent::Button::V : ButtonEvent::Button::CapitalV;
	case SDLK_w: return !is_shift_down ? ButtonEvent::Button::W : ButtonEvent::Button::CapitalW;
	case SDLK_x: return !is_shift_down ? ButtonEvent::Button::X : ButtonEvent::Button::CapitalX;
	case SDLK_y: return !is_shift_down ? ButtonEvent::Button::Y : ButtonEvent::Button::CapitalY;
	case SDLK_z: return !is_shift_down ? ButtonEvent::Button::Z : ButtonEvent::Button::CapitalZ;

	case SDLK_1: return !is_shift_down ? ButtonEvent::Button::One : ButtonEvent::Button::ExclaimationMark;
	case SDLK_2: return !is_shift_down ? ButtonEvent::Button::Two : ButtonEvent::Button::At;
	case SDLK_3: return !is_shift_down ? ButtonEvent::Button::Three : ButtonEvent::Button::Pound;
	case SDLK_4: return !is_shift_down ? ButtonEvent::Button::Four : ButtonEvent::Button::Dollar;
	case SDLK_5: return !is_shift_down ? ButtonEvent::Button::Five : ButtonEvent::Button::Percent;
	case SDLK_6: return !is_shift_down ? ButtonEvent::Button::Six : ButtonEvent::Button::Caret;
	case SDLK_7: return !is_shift_down ? ButtonEvent::Button::Seven : ButtonEvent::Button::And;
	case SDLK_8: return !is_shift_down ? ButtonEvent::Button::Eight : ButtonEvent::Button::Asterisk;
	case SDLK_9: return !is_shift_down ? ButtonEvent::Button::Nine : ButtonEvent::Button::LeftParenthesis;
	case SDLK_0: return !is_shift_down ? ButtonEvent::Button::Zero : ButtonEvent::Button::RightParenthesis;

	case SDLK_MINUS: return !is_shift_down ? ButtonEvent::Button::Minus : ButtonEvent::Button::Underscore;
	case SDLK_EQUALS: return !is_shift_down ? ButtonEvent::Button::Equals : ButtonEvent::Button::Plus;
	case SDLK_QUOTE: return !is_shift_down ? ButtonEvent::Button::Apostrophe : ButtonEvent::Button::QuotationMark;
	case SDLK_COMMA: return ButtonEvent::Button::Comma;
	case SDLK_PERIOD: return ButtonEvent::Button::Period;
	case SDLK_SLASH: return !is_shift_down ? ButtonEvent::Button::ForwardSlash : ButtonEvent::Button::QuestionMark;

	case SDLK_F1: return ButtonEvent::Button::F1;
	case SDLK_F2: return ButtonEvent::Button::F2;
	case SDLK_F3: return ButtonEvent::Button::F3;
	case SDLK_F4: return ButtonEvent::Button::F4;
	case SDLK_F5: return ButtonEvent::Button::F5;
	case SDLK_F6: return ButtonEvent::Button::F6;
	case SDLK_F7: return ButtonEvent::Button::F7;
	case SDLK_F8: return ButtonEvent::Button::F8;
	case SDLK_F9: return ButtonEvent::Button::F9;
	case SDLK_F10: return ButtonEvent::Button::F10;
	case SDLK_F11: return ButtonEvent::Button::F11;
	case SDLK_F12: return ButtonEvent::Button::F12;

	case SDLK_LSHIFT: 
	case SDLK_RSHIFT: return ButtonEvent::Button::Shift;
	case SDLK_LALT: 
	case SDLK_RALT: return ButtonEvent::Button::Alt;
	case SDLK_LCTRL: 
	case SDLK_RCTRL: return ButtonEvent::Button::Ctrl;

	case SDLK_ESCAPE: return ButtonEvent::Button::Esc;
	case SDLK_SPACE: return ButtonEvent::Button::Space;
	case SDLK_DELETE: return ButtonEvent::Button::Del;
	case SDLK_BACKSPACE: return ButtonEvent::Button::Backspace;

	default: return ButtonEvent::Button::None;
	}
}

ButtonEvent::Button InputSystem::SDL_ScancodeToButton(int sdl_scancode)
{
	const Uint8 *keys= SDL_GetKeyboardState(nullptr);
	bool is_shift_down= keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT];

	switch(sdl_scancode)
	{
	case SDL_SCANCODE_A: return !is_shift_down ? ButtonEvent::Button::A : ButtonEvent::Button::CapitalA;
	case SDL_SCANCODE_B: return !is_shift_down ? ButtonEvent::Button::B : ButtonEvent::Button::CapitalB;
	case SDL_SCANCODE_C: return !is_shift_down ? ButtonEvent::Button::C : ButtonEvent::Button::CapitalC;
	case SDL_SCANCODE_D: return !is_shift_down ? ButtonEvent::Button::D : ButtonEvent::Button::CapitalD;
	case SDL_SCANCODE_E: return !is_shift_down ? ButtonEvent::Button::E : ButtonEvent::Button::CapitalE;
	case SDL_SCANCODE_F: return !is_shift_down ? ButtonEvent::Button::F : ButtonEvent::Button::CapitalF;
	case SDL_SCANCODE_G: return !is_shift_down ? ButtonEvent::Button::G : ButtonEvent::Button::CapitalG;
	case SDL_SCANCODE_H: return !is_shift_down ? ButtonEvent::Button::H : ButtonEvent::Button::CapitalH;
	case SDL_SCANCODE_I: return !is_shift_down ? ButtonEvent::Button::I : ButtonEvent::Button::CapitalI;
	case SDL_SCANCODE_J: return !is_shift_down ? ButtonEvent::Button::J : ButtonEvent::Button::CapitalJ;
	case SDL_SCANCODE_K: return !is_shift_down ? ButtonEvent::Button::K : ButtonEvent::Button::CapitalK;
	case SDL_SCANCODE_L: return !is_shift_down ? ButtonEvent::Button::L : ButtonEvent::Button::CapitalL;
	case SDL_SCANCODE_M: return !is_shift_down ? ButtonEvent::Button::M : ButtonEvent::Button::CapitalM;
	case SDL_SCANCODE_N: return !is_shift_down ? ButtonEvent::Button::N : ButtonEvent::Button::CapitalN;
	case SDL_SCANCODE_O: return !is_shift_down ? ButtonEvent::Button::O : ButtonEvent::Button::CapitalO;
	case SDL_SCANCODE_P: return !is_shift_down ? ButtonEvent::Button::P : ButtonEvent::Button::CapitalP;
	case SDL_SCANCODE_Q: return !is_shift_down ? ButtonEvent::Button::Q : ButtonEvent::Button::CapitalQ;
	case SDL_SCANCODE_R: return !is_shift_down ? ButtonEvent::Button::R : ButtonEvent::Button::CapitalR;
	case SDL_SCANCODE_S: return !is_shift_down ? ButtonEvent::Button::S : ButtonEvent::Button::CapitalS;
	case SDL_SCANCODE_T: return !is_shift_down ? ButtonEvent::Button::T : ButtonEvent::Button::CapitalT;
	case SDL_SCANCODE_U: return !is_shift_down ? ButtonEvent::Button::U : ButtonEvent::Button::CapitalU;
	case SDL_SCANCODE_V: return !is_shift_down ? ButtonEvent::Button::V : ButtonEvent::Button::CapitalV;
	case SDL_SCANCODE_W: return !is_shift_down ? ButtonEvent::Button::W : ButtonEvent::Button::CapitalW;
	case SDL_SCANCODE_X: return !is_shift_down ? ButtonEvent::Button::X : ButtonEvent::Button::CapitalX;
	case SDL_SCANCODE_Y: return !is_shift_down ? ButtonEvent::Button::Y : ButtonEvent::Button::CapitalY;
	case SDL_SCANCODE_Z: return !is_shift_down ? ButtonEvent::Button::Z : ButtonEvent::Button::CapitalZ;

	case SDL_SCANCODE_1: return !is_shift_down ? ButtonEvent::Button::One : ButtonEvent::Button::ExclaimationMark;
	case SDL_SCANCODE_2: return !is_shift_down ? ButtonEvent::Button::Two : ButtonEvent::Button::At;
	case SDL_SCANCODE_3: return !is_shift_down ? ButtonEvent::Button::Three : ButtonEvent::Button::Pound;
	case SDL_SCANCODE_4: return !is_shift_down ? ButtonEvent::Button::Four : ButtonEvent::Button::Dollar;
	case SDL_SCANCODE_5: return !is_shift_down ? ButtonEvent::Button::Five : ButtonEvent::Button::Percent;
	case SDL_SCANCODE_6: return !is_shift_down ? ButtonEvent::Button::Six : ButtonEvent::Button::Caret;
	case SDL_SCANCODE_7: return !is_shift_down ? ButtonEvent::Button::Seven : ButtonEvent::Button::And;
	case SDL_SCANCODE_8: return !is_shift_down ? ButtonEvent::Button::Eight : ButtonEvent::Button::Asterisk;
	case SDL_SCANCODE_9: return !is_shift_down ? ButtonEvent::Button::Nine : ButtonEvent::Button::LeftParenthesis;
	case SDL_SCANCODE_0: return !is_shift_down ? ButtonEvent::Button::Zero : ButtonEvent::Button::RightParenthesis;

	case SDL_SCANCODE_MINUS: return !is_shift_down ? ButtonEvent::Button::Minus : ButtonEvent::Button::Underscore;
	case SDL_SCANCODE_EQUALS: return !is_shift_down ? ButtonEvent::Button::Equals : ButtonEvent::Button::Plus;
	case SDL_SCANCODE_APOSTROPHE: return !is_shift_down ? ButtonEvent::Button::Apostrophe : ButtonEvent::Button::QuotationMark;
	case SDL_SCANCODE_COMMA: return ButtonEvent::Button::Comma;
	case SDL_SCANCODE_PERIOD: return ButtonEvent::Button::Period;
	case SDL_SCANCODE_SLASH: return !is_shift_down ? ButtonEvent::Button::ForwardSlash : ButtonEvent::Button::QuestionMark;

	case SDL_SCANCODE_F1: return ButtonEvent::Button::F1;
	case SDL_SCANCODE_F2: return ButtonEvent::Button::F2;
	case SDL_SCANCODE_F3: return ButtonEvent::Button::F3;
	case SDL_SCANCODE_F4: return ButtonEvent::Button::F4;
	case SDL_SCANCODE_F5: return ButtonEvent::Button::F5;
	case SDL_SCANCODE_F6: return ButtonEvent::Button::F6;
	case SDL_SCANCODE_F7: return ButtonEvent::Button::F7;
	case SDL_SCANCODE_F8: return ButtonEvent::Button::F8;
	case SDL_SCANCODE_F9: return ButtonEvent::Button::F9;
	case SDL_SCANCODE_F10: return ButtonEvent::Button::F10;
	case SDL_SCANCODE_F11: return ButtonEvent::Button::F11;
	case SDL_SCANCODE_F12: return ButtonEvent::Button::F12;
	

	case SDL_SCANCODE_LSHIFT: 
	case SDL_SCANCODE_RSHIFT: return ButtonEvent::Button::Shift;
	case SDL_SCANCODE_LALT: 
	case SDL_SCANCODE_RALT: return ButtonEvent::Button::Alt;
	case SDL_SCANCODE_LCTRL: 
	case SDL_SCANCODE_RCTRL: return ButtonEvent::Button::Ctrl;

	case SDL_SCANCODE_ESCAPE: return ButtonEvent::Button::Esc;
	case SDL_SCANCODE_SPACE: return ButtonEvent::Button::Space;
	case SDL_SCANCODE_DELETE: return ButtonEvent::Button::Del;
	case SDL_SCANCODE_BACKSPACE: return ButtonEvent::Button::Backspace;

	default: return ButtonEvent::Button::None;
	}
}

MouseEvent::MouseButton InputSystem::SDLMouseButtonToButton(int sdl_mouse_button_value)
{
	switch(sdl_mouse_button_value)
	{
	case SDL_BUTTON_LEFT: return MouseEvent::MouseButton::Left;
	case SDL_BUTTON_MIDDLE: return MouseEvent::MouseButton::Middle;
	case SDL_BUTTON_RIGHT: return MouseEvent::MouseButton::Right;
	default: return MouseEvent::MouseButton::None;
	}
}

vector<MouseEvent::MouseButton> InputSystem::GetDownMouseButtons(unsigned int state)
{
	vector<MouseEvent::MouseButton> down_mouse_buttons;

	for(int i= SDL_BUTTON_LEFT; i<= SDL_BUTTON_RIGHT; i++)
		if(state& SDL_BUTTON(i))
		{
			MouseEvent::MouseButton button= SDLMouseButtonToButton(i);
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
	if(quit_requested)
		return false;

	float elapsed_seconds= timer->Stop();
	timer->Start();

	SDL_PumpEvents();

	Vec2i mouse_position;
	Uint32 mouse_state= SDL_GetMouseState(&mouse_position.c[0], &mouse_position.c[1]);
	Vec2f normalized_mouse_position= GetNormalizedMousePosition(mouse_position);
	Vec2f mouse_displacement= normalized_mouse_position- GetNormalizedMousePosition(last_mouse_position);

	SDL_Event sdl_event;
	while(SDL_PollEvent(&sdl_event))
	{
		if(sdl_event.type== SDL_QUIT)
			return false;
		
		vector<MouseEvent::MouseButton> down_mouse_buttons;

		switch(sdl_event.type)
		{
		case SDL_MOUSEWHEEL:
			BroadcastEvent(new MouseScrollEvent(elapsed_seconds, normalized_mouse_position, sdl_event.wheel.y));
			break;

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			BroadcastEvent(new MouseButtonEvent(elapsed_seconds, 
												SDLMouseButtonToButton(sdl_event.button.button), 
												sdl_event.button.type== SDL_MOUSEBUTTONUP ? MouseButtonEvent::Type::Up : MouseButtonEvent::Type::Down, 
												normalized_mouse_position));
			break;

		case SDL_MOUSEMOTION:
			BroadcastEvent(new MouseMotionEvent(elapsed_seconds, 
												normalized_mouse_position, 
												mouse_displacement));

			down_mouse_buttons= GetDownMouseButtons(sdl_event.motion.state);
			for(unsigned int i= 0; i< down_mouse_buttons.size(); i++)
				BroadcastEvent(new MouseDragEvent(elapsed_seconds, 
												  normalized_mouse_position,
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


	vector<MouseEvent::MouseButton> down_mouse_buttons= GetDownMouseButtons(mouse_state);
	for(unsigned int i= 0; i< down_mouse_buttons.size(); i++)
		BroadcastEvent(new MouseButtonEvent(elapsed_seconds, 
											down_mouse_buttons[i], 
											MouseButtonEvent::Type::Hold, 
											normalized_mouse_position));

	int key_count;
	const Uint8 *keys= SDL_GetKeyboardState(&key_count);

	for(int i= 0; i< key_count; i++)
	{
		if(keys[i])
		{
			ButtonEvent::Button button= SDL_ScancodeToButton(i);
			if(button!= ButtonEvent::Button::None)
				BroadcastEvent(new ButtonEvent(elapsed_seconds, 
					                           button, 
					                           ButtonEvent::Type::Hold));
		}
	}
	

	last_mouse_position= mouse_position;

	return true;
}

bool InputSystem::IsKeyDown(ButtonEvent::Button button)
{
	int key_count;
	const Uint8 *keys= SDL_GetKeyboardState(&key_count);

	for(int i= 0; i< key_count; i++)
	{
		if(SDL_ScancodeToButton(i)== button)
		{
			if(keys[i])
				return true;
			else
				return false;
		}
	}

	return false;
}

void InputSystem::Quit()
{
	quit_requested= true;
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
