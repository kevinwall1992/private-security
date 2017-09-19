#ifndef PS_LIST_PANE
#define PS_LIST_PANE

#include "Pane.h"
#include "Button.h"
#include "Common.h"
#include "GrayscaleMaskGenerator.h"


class ListPane : public Pane
{
public:
	enum Direction { LeftToRight, RightToLeft, TopToBottom, BottomToTop };

private:
	vector<Pane *> panes;
	Direction direction;
	int visual_length;
	float margin= 0.01f;

	Ninepatch selection_box;
	Pane *selected= nullptr;

	Ninepatch frame;
	bool frame_content= false;
	GrayscaleMaskGenerator grayscale_mask_generator;

	float scroll_displacement= 0;
	float scroll_speed= 1;

	void PositionPanes();

protected:
	virtual void ResizeSelectionBox();

public:
	ListPane(Direction direction, float scroll_speed= 0, bool frame_content= false);

	void Select(Pane *pane);
	void Deselect();
	Pane * GetSelected();

	virtual void AddPane(Pane *pane);
	virtual void RemovePane(Pane *pane, bool delete_pane= false);

	virtual void ClearPanes(bool delete_panes= false);
	virtual vector<Pane *> GetPanes();
	template<class T>
	vector<T *> GetPanesOfType()
	{
		vector<T *> t_panes;

		for(Pane *pane : GetPanes())
			if(Utility::IsType<T>(pane))
				t_panes.push_back(dynamic_cast<T *>(pane));

		return t_panes;
	}

	virtual void ResizePanes(int pane_count_per_page= -1, float pane_thickness= 1.0f);

	void FrameContent();
	void DontFrameContent();

	void SetScrollSpeed(float scroll_speed);

	virtual void MouseScrollOver(int scroll_amount);

	virtual void Draw();
};

class GridPane : public ListPane
{
	int column_count;
	int row_count;

	vector<ListPane *> GetListPanes();

	using ListPane::ResizePanes;

	void ResizeGridElements();

public:
	GridPane(int column_count, int row_count);

	void AddPane(Pane *pane);
	void RemovePane(Pane *pane, bool delete_pane= false);

	vector<Pane *> GetPanes();
};

#endif
