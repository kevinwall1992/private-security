#ifndef PS_UI
#define PS_UI

#include "Pane.h"
#include "Gizmos.h"
#include "Button.h"

//High level user interface objects


class TacticalPane : public CameraPane
{
	CameraGizmo *camera_gizmo;
	Button button;
	
	int level= 0;

	Tile *hovered_tile;
	Actor *selected_actor= nullptr;

	VAO tile_frame_vao;
	static float tile_frame_vertices[24* 3];

	Tile * GetTile(Ray ray);

	bool is_hovered= false;

protected:
	void HandleEvent(Event *event_);

public:
	TacticalPane(Camera *camera);

	virtual void MouseLeftUpRay(Ray ray);
	virtual void MouseMotionRay(Ray ray, Ray last_ray);
	virtual void MouseScroll(int scroll_amount);

	virtual void MouseIn();
	virtual void MouseOut();

	virtual void Draw();
};

#endif
