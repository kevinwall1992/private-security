#include "ListPane.h"

void ListPane::PositionPanes()
{
	float pane_size_in_list_direction= 1.0f/ visual_length;
	
	for(unsigned int i= 0; i< panes.size(); i++)
	{
		Pane *pane= panes[i];
		
		float pane_offset_in_list_direction;
		switch(direction)
		{
		case LeftToRight: 
		case BottomToTop: 
			pane_offset_in_list_direction= i* pane_size_in_list_direction+ margin* (1+ i); 
			break;

		case RightToLeft:
		case TopToBottom: 
			pane_offset_in_list_direction= (panes.size()- (i+ 1))* pane_size_in_list_direction- margin* (1+ i); 
			break;

		default: break;
		}

		switch(direction)
		{
		case LeftToRight: 
		case RightToLeft: 
			pane->Size.x= pane_size_in_list_direction- margin;
			pane->Offset.x= pane_offset_in_list_direction;
			break;

		case TopToBottom: 
		case BottomToTop: 
			pane->Size.y= pane_size_in_list_direction- margin;
			pane->Offset.y= pane_offset_in_list_direction;
			break;

		default: break;
		}
	}
}

ListPane::ListPane(Direction direction_, int visual_length)
{
	direction= direction_;
	SetVisualLength(visual_length);
}

ListPane::ListPane()
{
	direction= Direction::TopToBottom;
	SetVisualLength(5);
}

void ListPane::SetVisualLength(int visual_length_)
{
	visual_length= visual_length_;
	PositionPanes();
}

void ListPane::AddPane(Pane *pane)
{
	panes.push_back(pane);
	PositionPanes();

	AddComponent(pane);
}

ButtonListPane::ButtonListPane(Direction direction, int visual_length)
	: ListPane(direction, visual_length)
{

}

ButtonListPane::ButtonListPane()
{

}

void ButtonListPane::AddButton(Button *button)
{
	AddPane(button);
}
