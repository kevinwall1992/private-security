#ifndef PS_UI
#define PS_UI

#include "Pane.h"
#include "Gizmos.h"
#include "Button.h"
#include "Menu.h"
#include "Dialog.h"
#include "ListPane.h"


//High level user interface objects

class AboutDialog : public Dialog
{
public:
	AboutDialog();
};


class AboutButton : public OpenButton
{
public:
	AboutButton();
};

class QuitButton : public TextButton
{
public:
	QuitButton();

	virtual void MouseLeftClick();
};

class ResumeButton : public BackButton
{
public:
	ResumeButton();
};

class SaveButton : public TextButton
{
public:
	SaveButton();

	virtual void MouseLeftClick();
};

class LoadButton : public TextButton
{
public:
	LoadButton();

	virtual void MouseLeftClick();
};


class PauseMenu : public Menu
{
	ButtonListPane button_list_pane;

	AboutButton about_button;
	QuitButton quit_button;
	ResumeButton resume_button;
	SaveButton save_button;
	LoadButton load_button;

	TextPane::Group button_group;

public:
	PauseMenu();
};


class TacticalInterface : public CameraPane
{
	CameraGizmo *camera_gizmo;
	
	int level= 0;

	Tile *hovered_tile;
	Actor *selected_actor= nullptr;

	VAO tile_frame_vao;
	static float tile_frame_vertices[24* 3];

	Tile * GetTile(Ray ray);

protected:
	void HandleEvent(Event *event_);

public:
	TacticalInterface(Camera *camera);

	virtual void KeyUp(ButtonType button);

	virtual void MouseLeftUpRay(Ray ray);
	virtual void MouseMotionRay(Ray ray, Ray last_ray);
	virtual void MouseScroll(int scroll_amount);

	virtual void MouseIn();
	virtual void MouseOut();

	virtual void Draw();
};

#endif
