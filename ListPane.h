#ifndef PS_LIST_PANE
#define PS_LIST_PANE

#include "Pane.h"
#include "Button.h"
#include "Common.h"


class ListPane : public Pane
{
public:
	enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop };

private:
	vector<Pane *> panes;
	Direction direction;
	int visual_length;
	float margin= 0.01f;

	void PositionPanes();

public:
	ListPane(Direction direction, int visual_length= 5);
	ListPane();

	void SetVisualLength(int visual_length);

	void AddPane(Pane *pane);

	virtual void Draw();
};

class ButtonListPane : public ListPane
{
	using ListPane::AddPane;

public:
	ButtonListPane(Direction direction, int visual_length= 5);
	ButtonListPane();

	void AddButton(Button *button);
};

#endif
