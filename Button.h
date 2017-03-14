#ifndef PS_BUTTON
#define PS_BUTTON

#include "Pane.h"
#include "Ninepatch.h"

class Button : public Pane
{
	Ninepatch ninepatch;
	Pane *content;

public:
	typedef Ninepatch::Roundness Roundness;

	Button(string image_filename, Roundness roundness= Roundness::Normal, Pane *content= nullptr);

	void SetContent(Pane *content);

	virtual void Draw();
};

#endif