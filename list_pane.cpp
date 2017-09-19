#include "ListPane.h"


void ListPane::PositionPanes()
{
	//float pane_size_in_list_direction= 1.0f/ visual_length- margin/ visual_length;
	float orthogonal_margin= margin* GetAspectRatio();
	
	float cumulative_offset= scroll_displacement;
	for(unsigned int i= 0; i< panes.size(); i++)
	{
		Pane *pane= panes[i];
		float pane_size_in_list_direction= ((direction== Direction::LeftToRight || direction== Direction::RightToLeft) ? pane->Size.x : pane->Size.y)+ margin;
		
		float pane_offset_in_list_direction;
		switch(direction)
		{
		case LeftToRight: 
		case BottomToTop: 
			pane_offset_in_list_direction= cumulative_offset; 
			break;

		case RightToLeft:
		case TopToBottom: 
			pane_offset_in_list_direction= 1- (pane_size_in_list_direction+ cumulative_offset); 
			break;

		default: break;
		}

		cumulative_offset+= pane_size_in_list_direction;

		switch(direction)
		{
		case LeftToRight: 
		case RightToLeft: 
			pane->Offset.x= pane_offset_in_list_direction;
			//pane->Offset.y= (orthogonal_margin+ orthogonal_margin/ visual_length)/ 2;
			break;

		case TopToBottom: 
		case BottomToTop: 
			pane->Offset.y= pane_offset_in_list_direction;
			//pane->Offset.x= (orthogonal_margin+ orthogonal_margin/ visual_length)/ 2;
			break;

		default: break;
		}
	}
}

void ListPane::ResizeSelectionBox()
{
	if(selected!= nullptr)
	{
		selection_box.Size= GlobalSizeToLocalSize(selected->GetGlobalSize())* 1.1f;
		selection_box.Offset= GlobalPositionToLocalPosition(selected->GetGlobalOffset())- (selection_box.Size.Get()* 0.05f);
	}
}

ListPane::ListPane(Direction direction_, float scroll_speed_, bool frame_content_)
	: frame("frame.png", Ninepatch::Roundness::Soft), 
	  grayscale_mask_generator(System::graphics.GetScreenSize()), 
	  selection_box("selection.png", Ninepatch::Roundness::Normal)
{
	direction= direction_;

	scroll_speed= scroll_speed_;

	frame_content= frame_content_;
	if(!frame_content)
		frame.Hide();
	frame.TreeFrame::SetParent(this);
	AddComponent((Drawable *)&frame);

	selection_box.Hide();
	selection_box.MakeTransparentToMouseInput();
	AddComponent(&selection_box);
}

void ListPane::Select(Pane *pane)
{
	selected= pane;
	selection_box.Show();
}

void ListPane::Deselect()
{
	selected= nullptr;
	selection_box.Hide();
}

Pane * ListPane::GetSelected()
{
	return selected;
}

void ListPane::AddPane(Pane *pane)
{
	panes.push_back(pane);

	AddComponent(pane, true);
}

void ListPane::RemovePane(Pane *pane, bool delete_pane)
{
	for(unsigned int i= 0; i< panes.size(); i++)
		if(pane== panes[i])
			panes.erase(panes.begin()+ i--);

	RemoveComponent(pane, delete_pane);
}

void ListPane::ClearPanes(bool delete_panes)
{
	for(Pane *pane : panes)
		RemoveComponent(pane, delete_panes);

	panes.clear();
}

vector<Pane*> ListPane::GetPanes()
{
	return panes;
}

void ListPane::ResizePanes(int pane_count_per_page, float pane_thickness)
{
	if(pane_count_per_page== -1)
		pane_count_per_page= (int)panes.size();

	float space_minus_margins= (1.0f- (pane_count_per_page- 1)* margin);

	Vec2f size;
	if(direction== Direction::BottomToTop || direction== Direction::TopToBottom)
		size= Vec2f(pane_thickness, space_minus_margins/ pane_count_per_page);
	else
		size= Vec2f(space_minus_margins/ pane_count_per_page, pane_thickness);

	for(Pane *pane : panes)
	{
		pane->Size= size;
		pane->Offset= Vec2f(0, 0);
	}
}

void ListPane::FrameContent()
{
	frame_content= true;
	frame.Show();
}

void ListPane::DontFrameContent()
{
	frame_content= false;
	frame.Hide();
}

void ListPane::SetScrollSpeed(float scroll_speed_)
{
	scroll_speed= scroll_speed_;
}

void ListPane::MouseScrollOver(int scroll_amount)
{
	scroll_displacement+= 3.0f* scroll_speed* scroll_amount/ 100.0f;
}

void ListPane::Draw()
{
	PositionPanes();

	ResizeSelectionBox();

	if(frame_content)
	{
		Texture current_grayscale_mask= Texture::GetBoundTexture(8);
		grayscale_mask_generator.SetSourceTexture(current_grayscale_mask);
		Texture grayscale_mask= grayscale_mask_generator.GenerateGrayscaleMask_Quad(GetTransform());
		grayscale_mask.BindToIndex(8);

		Pane::Draw();

		current_grayscale_mask.BindToIndex(8);
	}
	else
		Pane::Draw();
}


vector<ListPane *> GridPane::GetListPanes()
{
	vector<ListPane *> list_panes;

	for(Pane *pane : ListPane::GetPanes())
		list_panes.push_back(dynamic_cast<ListPane *>(pane));

	return list_panes;
}

void GridPane::ResizeGridElements()
{
	for(ListPane *list_pane : GetListPanes())
		list_pane->ResizePanes(column_count);

	ListPane::ResizePanes(row_count);
}

GridPane::GridPane(int column_count_, int row_count_)
	: ListPane(Direction::TopToBottom)
{
	column_count= column_count_;
	row_count= row_count_;
}

void GridPane::AddPane(Pane *pane)
{
	bool pane_added= false;

	for(ListPane *list_pane : GetListPanes())
	{
		if(list_pane->GetPanes().size()< column_count)
		{
			list_pane->AddPane(pane);
			pane_added= true;
			break;
		}
	}

	if(!pane_added)
	{
		ListPane *list_pane= new ListPane(Direction::LeftToRight);
		list_pane->AddPane(pane);

		ListPane::AddPane(list_pane);
	}

	ResizeGridElements();
}

void GridPane::RemovePane(Pane *target_pane, bool delete_pane)
{
	vector<Pane *> panes= GetPanes();

	for(unsigned int i= 0; i< panes.size(); i++)
		if(panes[i]== target_pane)
			panes.erase(panes.begin()+ i--);

	vector<ListPane *> list_panes= GetListPanes();
	for(ListPane *list_pane : list_panes)
		ListPane::RemovePane(list_pane, true);

	for(Pane *pane : panes)
		AddPane(pane);

	ResizeGridElements();
}

vector<Pane *> GridPane::GetPanes()
{
	vector<Pane *> panes;

	for(ListPane *list_pane : GetListPanes())
		for(Pane *pane : list_pane->GetPanes())
			panes.push_back(pane);

	return panes;
}