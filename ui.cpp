#include "UI.h"
#include "InputSystem.h"
#include "GameSystem.h"
#include "Viewport.h"
#include "Pathing.h"
#include "Actions.h"
#include "Direction.h"


AboutDialog::AboutDialog()
	: Dialog("nine.png", "Private Security\nv"+ Utility::TextFileToString("data/other/version.txt"))
{
	Size= Vec2f(0.3f, 0.2f);
	Offset= Vec2f(0.35f, 0.4f);
}


void PauseMenu::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		Button *button= dynamic_cast<ButtonInterfaceEvent *>(interface_event)->GetButton();

		if(button== &about_button)
		{
			Menu::about_dialog->Open();
			return;
		}
		else if(button== &quit_button)
		{
			System::input.Quit();
			return;
		}
		else if(button== &resume_button)
		{
			Back();
			return;
		}
		else if(button== &save_button)
		{
			SaveFileDialog *save_file_dialog= new SaveFileDialog(save_load_filepath, this);
			save_file_dialog->Open();
			return;
		}
		else if(button== &load_button)
		{
			LoadFileDialog *load_file_dialog= new LoadFileDialog(save_load_filepath, this, Utility::MakeVector<string>("xml"));
			load_file_dialog->Open();
			return;
		}
	}

	Menu::HandleInterfaceEvent(interface_event);
}

PauseMenu::PauseMenu()
	: button_list_pane(ListPane::Direction::TopToBottom), 
	  about_button("nine.png", "About", Color::Black), 
	  quit_button("nine.png", "Quit", Color::Black), 
	  resume_button("nine.png", "Resume", Color::Black), 
	  save_button("nine.png", "Save", Color::Black), 
	  load_button("nine.png", "Load", Color::Black)
{
	button_list_pane.AddPane(&about_button);
	button_list_pane.AddPane(&save_button);
	button_list_pane.AddPane(&load_button);
	button_list_pane.AddPane(&resume_button);
	button_list_pane.AddPane(&quit_button);

	AddComponent(&button_list_pane);

	Size= Vec2f(0.3f, 0.85f);
	Offset= Vec2f(0.35f, 0.0f);


	for(Pane *pane : button_list_pane.GetPanes())
		button_group.AddTextPane(dynamic_cast<TextButton *>(pane)->GetTextPane());

	button_list_pane.ResizePanes(10);
}

void PauseMenu::SetSaveLoadFilepath(string save_load_filepath_)
{
	save_load_filepath= save_load_filepath_;
}

void PauseMenu::AddButton(Button *button)
{
	button_list_pane.AddPane(button);
	if(Utility::IsType<TextButton>(button))
		button_group.AddTextPane(dynamic_cast<TextButton *>(button)->GetTextPane());

	button_list_pane.ResizePanes(8);
}


void SaveFileDialog::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		filepath= Utility::GetDirectoryPath(filepath)+ filename_textbox.GetText();

		SpawnInterfaceEvent(new FilepathSelectedEvent(this));

		Menu::Back();
		Menu::Back();
	}
}

SaveFileDialog::SaveFileDialog(string initial_filepath, Interface * interface_event_recipient)
	: Menu(true),
	  list_pane(ListPane::Direction::TopToBottom), 
	  background("nine.png", Ninepatch::Roundness::Normal), 
	  prompt_text("Enter filename to save as.", Color::Black),
	  filename_textbox(),
	  save_button("nine.png", "Save", Color::Black)
{
	Size= Vec2f(0.3f, 0.5f);
	Offset= (Vec2f(1, 1)- Size)/ 2;

	filepath= initial_filepath;
	filename_textbox.SetText(GetFilename());

	AddInterfaceEventRecipient(interface_event_recipient);

	list_pane.AddPane(&prompt_text);
	list_pane.AddPane(&filename_textbox);
	list_pane.AddPane(&save_button);
	list_pane.ResizePanes();

	AddComponent(&background);
	AddComponent(&list_pane);
}

string SaveFileDialog::GetFilepath()
{
	return filepath;
}

string SaveFileDialog::GetFilename()
{
	return Utility::GetFilename(filepath);
}


SaveFileDialog::FilepathSelectedEvent::FilepathSelectedEvent(SaveFileDialog *save_file_dialog)
	: InterfaceEvent(save_file_dialog)
{

}

string SaveFileDialog::FilepathSelectedEvent::GetFilepath()
{
	return GetInterfaceAs<SaveFileDialog>()->GetFilepath();
}

string SaveFileDialog::FilepathSelectedEvent::GetFilename()
{
	return GetInterfaceAs<SaveFileDialog>()->GetFilename();
}


void LoadFileDialog::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		if(dynamic_cast<ButtonInterfaceEvent *>(interface_event)->GetButton()!= &load_button)
			return;

		SpawnInterfaceEvent(new FilepathSelectedEvent(this));

		Menu::Back();
		Menu::Back();
	}
}

LoadFileDialog::LoadFileDialog(string initial_filepath, Interface *interface_event_recipient, vector<string> file_extensions)
	: Menu(true), 
	  select_file_button(initial_filepath, file_extensions), 
	  load_button("nine.png", "Load", Color::Black),
	  list_pane(ListPane::Direction::TopToBottom)
{
	Size= Vec2f(0.3f, 0.5f);
	Offset= (Vec2f(1, 1)- Size)/ 2;

	list_pane.AddPane(&select_file_button);
	list_pane.AddPane(&load_button);
	list_pane.ResizePanes();

	AddComponent(&list_pane);

	AddInterfaceEventRecipient(interface_event_recipient);
}

string LoadFileDialog::GetFilepath()
{
	return select_file_button.GetFilepath();
}

string LoadFileDialog::GetFilename()
{
	return select_file_button.GetFilename();
}


LoadFileDialog::FilepathSelectedEvent::FilepathSelectedEvent(LoadFileDialog *load_file_dialog)
	: InterfaceEvent(load_file_dialog)
{

}

string LoadFileDialog::FilepathSelectedEvent::GetFilepath()
{
	return GetInterfaceAs<LoadFileDialog>()->GetFilepath();
}

string LoadFileDialog::FilepathSelectedEvent::GetFilename()
{
	return GetInterfaceAs<LoadFileDialog>()->GetFilename();
}



IconCamera * IconPane::icon_camera= nullptr;

IconPane::IconPane(Mesh *mesh, float scale_)
	: cycle(4.0f, 0.5f)
{
	scene.AddLight(new PointLight(Vec3f(3.0f, 10.0f, 5.0f), Color::White));

	SetMesh(mesh);

	scale= scale_;
}

float IconPane::GetScale()
{
	return scale;
}

void IconPane::SetScale(float scale_)
{
	scale= scale_;
}

Mesh * IconPane::GetMesh()
{
	return mesh_prop->GetMesh();
}

void IconPane::SetMesh(Mesh *mesh)
{
	scene.ClearProps();

	if(mesh!= nullptr)
	{
		mesh_prop= new MeshProp(mesh);
		scene.AddProp(mesh_prop);
	}
	else
		mesh_prop= nullptr;
}

void IconPane::MouseIn()
{
	cycle.Start();
}

void IconPane::MouseOut()
{
	cycle.Stop();
}

Texture IconPane::GetTexture()
{
	const float default_fov= Math::DegreesToRadians(35);

	if(icon_camera== nullptr)
		icon_camera= new IconCamera(default_fov, Vec3f(3, 3, 5), Vec3f(0, 0.5f, 0));
	icon_camera->FOV= default_fov/ scale;

	mesh_prop->SetRotation((float)(2* M_PI* cycle.GetMoment()));

	Viewport::Push();
	Framebuffer framebuffer= Framebuffer::GetCurrentFramebuffer();

	int width= System::graphics.GetScreenSize().x/ 5;
	Texture texture= icon_camera->TakePhoto(scene, Vec2i(width, width), Photo::Type::FullColor).GetTexture();

	framebuffer.Bind();
	Viewport::Pop();

	return texture;
}

void IconPane::Draw()
{
	if(mesh_prop== nullptr)
		return;

	TexturePane::Draw();
}

ToolbeltSlot::ToolbeltSlot()
	: Button("nine.png")
{
	
}

void ToolbeltSlot::SetItem(Item *item_)
{
	if(item_== item || item_== nullptr)
		return;

	item= item_;

	SetContent(new IconPane(item->GetIconMesh()));
	GetContent()->Size= Vec2f(0.9f, 0.9f);
	GetContent()->Offset= Vec2f(0.05f, 0.05f);
}

Item * ToolbeltSlot::GetItem()
{
	return item;
}

Toolbelt::Toolbelt()
	: ListPane(Direction::LeftToRight)
{
	for(unsigned int i= 0; i< 10; i++)
		AddPane(slots+ i);
	ResizePanes(10);

	Size= Vec2f(0.8f, 0.08f);
	Offset= Vec2f(0.1f, 0.1f);
}

void Toolbelt::SetActor(Actor *actor_)
{
	actor= actor_;
}

void Toolbelt::Draw()
{
	if(actor!= nullptr)
	{
		Inventory inventory= actor->GetInventory();

		for(unsigned int i= 0; i< inventory.size(); i++)
			slots[i].SetItem(inventory[i]);
	}

	ListPane::Draw();
}


float _3DInterfacePane::tile_frame_vertices[24* 3]= { -0.5f, 0.0f, -0.5f,  +0.5f, 0.0f, -0.5f, 
															  +0.5f, 0.0f, -0.5f,  +0.5f, 0.0f, +0.5f, 
															  +0.5f, 0.0f, +0.5f,  -0.5f, 0.0f, +0.5f, 
															  -0.5f, 0.0f, +0.5f,  -0.5f, 0.0f, -0.5f, 

															  -0.5f, 1.0f, -0.5f,  +0.5f, 1.0f, -0.5f, 
															  +0.5f, 1.0f, -0.5f,  +0.5f, 1.0f, +0.5f, 
															  +0.5f, 1.0f, +0.5f,  -0.5f, 1.0f, +0.5f, 
															  -0.5f, 1.0f, +0.5f,  -0.5f, 1.0f, -0.5f, 

															  -0.5f, 0.0f, -0.5f,  -0.5f, 1.0f, -0.5f, 
															  +0.5f, 0.0f, -0.5f,  +0.5f, 1.0f, -0.5f, 
															  +0.5f, 0.0f, +0.5f,  +0.5f, 1.0f, +0.5f, 
															  -0.5f, 0.0f, +0.5f,  -0.5f, 1.0f, +0.5f };

Tile * _3DInterfacePane::GetTile(Ray ray)
{
	Vec3f normal= Vec3f(0, 1, 0);
	Vec3i intersection_point= ray.tail+ ray.direction* (normal* (float)level- ray.tail).Dot(normal)/ ray.direction.Dot(normal);
	intersection_point.y= level;

	return System::game.space.GetTile(intersection_point);
}

_3DInterfacePane::_3DInterfacePane(Camera *camera)
	: CameraPane(camera)
{
	GLuint position_attribute_location= ShaderProgram::Retrieve("lines.program")->GetAttributeLocation("position");

	GLuint tile_frame_vertex_buffer_handle;
	glGenBuffers(1, &tile_frame_vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, tile_frame_vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, 24* 3* sizeof(float), tile_frame_vertices, GL_STATIC_DRAW);
	tile_frame_vao.SetAttributeBinding3f(position_attribute_location);

	glGenBuffers(1, &movement_graph_vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, movement_graph_vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, 10000* 26* 2* 3* sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	movement_graph_vao.SetAttributeBinding3f(position_attribute_location);

	Utility::HandleOpenGLErrors();

	camera_gizmo= new CameraGizmo(camera);
	AddComponent(camera_gizmo);
}

Tile * _3DInterfacePane::GetHoveredTile()
{
	return hovered_tile;
}

Tile * _3DInterfacePane::GetSelectedTile(bool mouse_left)
{
	return mouse_left ? selected_tile_left : selected_tile_right;
}

Actor * _3DInterfacePane::GetSelectedActor(bool mouse_left)
{
	return mouse_left ? selected_actor_left : selected_actor_right;
}

void _3DInterfacePane::AddHighlightedTile(Tile * tile, Color highlight_color)
{
	highlighted_tiles[tile]= highlight_color;
}

void _3DInterfacePane::ClearHighlightedTiles()
{
	highlighted_tiles.clear();
}

void _3DInterfacePane::HighlightSelectedTile()
{
	highlight_selected_tile= true;
}

void _3DInterfacePane::DontHighlightSelectedTile()
{
	highlight_selected_tile= false;
}

void _3DInterfacePane::LockSelection()
{
	selection_is_locked= true;
}

void _3DInterfacePane::UnlockSelection()
{
	selection_is_locked= false;
}

void _3DInterfacePane::KeyUp(ButtonType button)
{
}

void _3DInterfacePane::MouseLeftUpRay(Ray ray)
{
	if(selection_is_locked)
		return;

	Tile *tile= GetTile(ray);
	selected_tile_left= tile;
	if(selected_tile_left!= nullptr)
	{
		MouseLeftTileSelected(selected_tile_left);

		if(selected_tile_left->HasActor())
		{
			selected_actor_left= selected_tile_left->GetActor();
			MouseLeftActorSelected(selected_actor_left);
		}

		SpawnInterfaceEvent(new TileSelectedEvent(this, tile));
	}
}

void _3DInterfacePane::MouseRightUpRay(Ray ray)
{
	if(selection_is_locked)
		return;

	Tile *tile= GetTile(ray);
	selected_tile_right= tile;
	if(selected_tile_right!= nullptr)
	{
		MouseRightTileSelected(selected_tile_right);

		if(selected_tile_right->HasActor())
		{
			selected_actor_right= selected_tile_right->GetActor();
			MouseRightActorSelected(selected_actor_right);
		}
	}
}

void _3DInterfacePane::MouseMotionRay(Ray ray, Ray last_ray)
{
	if(selection_is_locked)
		return;

	Tile *tile= GetTile(ray);

	if(tile!= nullptr)
	{
		hovered_tile= tile;
		SpawnInterfaceEvent(new TileHoveredEvent(this, tile));
	}
}

void _3DInterfacePane::MouseScroll(int scroll_amount)
{
	if(selection_is_locked)
		return;

	if(hovered_tile!= nullptr)
	{
		Vec3i position= hovered_tile->GetPosition();
		Tile *tile= System::game.space.GetTile(position.x, position.z, level+ scroll_amount);

		if(tile!= nullptr)
		{
			level+= scroll_amount;
			hovered_tile= tile;
		}
	}
}

void _3DInterfacePane::MouseLeftTileSelected(Tile *tile)
{
	
}

void _3DInterfacePane::MouseRightTileSelected(Tile *tile)
{
}

void _3DInterfacePane::MouseLeftActorSelected(Actor *actor)
{
}

void _3DInterfacePane::MouseRightActorSelected(Actor *actor)
{
}

void _3DInterfacePane::Draw()
{
	CameraPane::Draw();


	ShaderProgram *shader_program= ShaderProgram::Retrieve("lines.program");
	shader_program->Use();
	shader_program->SetUniformMatrix4x4f("camera_transform", GetCamera()->GetProjectedTransform(Math::GetAspectRatio(GetPixelSize())).GetMatrix());
	shader_program->SetUniformVector3f("line_color", Color::Green);

	AttachDepth();
	Viewport::Push(GetPixelOffset(), GetPixelSize());
	tile_frame_vao.Bind();

	std::map<Tile *, Color> highlighted_tiles= this->highlighted_tiles;
	if(hovered_tile!= nullptr)
		highlighted_tiles[hovered_tile]= Color::Green;
	for(std::pair<Tile *, Color> tile_pair : highlighted_tiles)
	{
		Vec3f tile_position= tile_pair.first->GetPosition();
		shader_program->SetUniformMatrix4x4f("model_transform", Transform().Translate(Vec3f(tile_position.x, tile_position.y, tile_position.z)+ Vec3f(0.5f, 0.0f, 0.5f)).GetMatrix());
		shader_program->SetUniformVector3f("line_color", tile_pair.second);

		glDrawArrays(GL_LINES, 0, 24);
	}

	movement_graph_vao.Bind();
	glBindBuffer(GL_ARRAY_BUFFER, movement_graph_vertex_buffer_handle);
	Vector<3, float> *buffer= (Vector<3, float> *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	int buffer_index= 0;

	Vec3i world_size= System::game.space.GetSize();
	for(int column_index= 0; column_index< world_size.x; column_index++)
	{
		for(int row_index= 0; row_index< world_size.z; row_index++)
		{
			Tile *tile= System::game.space.GetTile(column_index, row_index, level);

			vector<Move *> moves;
			for(int i= Direction::East; i<= Direction::Southeast; i++)
			{
				RelativeMove move(tile, (Direction::Enum)i, MoveType::Walk);

				if(move.GetDestination()!= nullptr && !tile->DoesBlock(&move) && !move.GetDestination()->DoesBlock(&move))
				{
					buffer[buffer_index++]= tile->GetPosition();
					buffer[buffer_index++]= move.GetDestination()->GetPosition();
				}
			}
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	shader_program->SetUniformMatrix4x4f("model_transform", Transform().Translate(Vec3f(0, 0, 0)+ Vec3f(0.5f, 0.5f, 0.5f)).GetMatrix());
	shader_program->SetUniformVector3f("line_color", Color::White);
	if(buffer_index> 0)
		glDrawArrays(GL_LINES, 0, buffer_index);

	Utility::HandleOpenGLErrors();

	Viewport::Pop();
}


_3DInterfacePane::TileSelectedEvent::TileSelectedEvent(_3DInterfacePane *_3d_interface_pane, Tile *selected_tile_)
	: InterfaceEvent(_3d_interface_pane)
{
	selected_tile= selected_tile_;
}

Tile * _3DInterfacePane::TileSelectedEvent::GetSelectedTile()
{
	return selected_tile;
}


_3DInterfacePane::TileHoveredEvent::TileHoveredEvent(_3DInterfacePane *_3d_interface_pane, Tile *hovered_tile_)
	: InterfaceEvent(_3d_interface_pane)
{
	hovered_tile= hovered_tile_;
}

Tile * _3DInterfacePane::TileHoveredEvent::GetHoveredTile()
{
	return hovered_tile;
}


void TacticalInterface::HandleInterfaceEvent(InterfaceEvent *interface_event)
{

}

TacticalInterface::TacticalInterface(Camera *camera)
	: _3DInterfacePane(camera)
{
	AddComponent(&toolbelt);
}

void TacticalInterface::MouseRightUpRay(Ray ray)
{
	Tile *tile= GetTile(ray);

	Actor *selected_actor= GetSelectedActor();
	if(tile!= nullptr && selected_actor!= nullptr && selected_actor->GetAction()== nullptr)
	{
		Path path= Path::GetPath(new Node(selected_actor, selected_actor->GetTile()), new Node(selected_actor, tile));

		Edge *edge;
		while((edge= path.PopEdge())!= nullptr)
			System::game.SubmitAction(new MoveAction(selected_actor, edge->GetMove()));
	}
}

void TacticalInterface::MouseLeftActorSelected(Actor *actor)
{
	toolbelt.SetActor(actor);
}

void TacticalInterface::Draw()
{
	_3DInterfacePane::Draw();

	Menu::Draw_Menus();
}


MapEditor::DetailPane::DetailPane()
{

}

MapEditor::DetailPaneBrowser * MapEditor::DetailPane::GetDetailPaneBrowser()
{
	return detail_pane_browser;
}

void MapEditor::DetailPane::SetDetailPaneBrowser(DetailPaneBrowser *detail_pane_browser_)
{
	detail_pane_browser= detail_pane_browser_;
}

void MapEditor::DetailPane::Close()
{
	if(detail_pane_browser!= nullptr && this== detail_pane_browser->GetCurrentDetailPane())
		detail_pane_browser->Back();
}


void MapEditor::DetailPaneBrowser::Open(DetailPane *detail_pane)
{
	if(detail_pane_stack.size()> 0)
		RemoveComponent(detail_pane_stack.top());

	detail_pane_stack.push(detail_pane);
	AddComponent(detail_pane);

	detail_pane->SetDetailPaneBrowser(this);
}

void MapEditor::DetailPaneBrowser::Back()
{
	if(detail_pane_stack.size()== 0)
		return;

	RemoveComponent(detail_pane_stack.top(), true);
	detail_pane_stack.pop();

	if(detail_pane_stack.size()> 0)
		AddComponent(detail_pane_stack.top());
}

MapEditor::DetailPane * MapEditor::DetailPaneBrowser::GetCurrentDetailPane()
{
	return detail_pane_stack.size()> 0 ? detail_pane_stack.top() : nullptr;
}


MapEditor::ObjectDetailPane::Element::Element()
	: Ninepatch("nine.png", Roundness::Normal)
{
}


void MapEditor::TileDetailPane::FillListPanes()
{
	item_list_pane.ClearPanes(true);

	for(Item *item : tile->GetItems())
		item_list_pane.AddPane(new ObjectIconButton(item, this));
	item_list_pane.ResizePanes(3);


	furniture_list_pane.ClearPanes(true);

	for(Furniture *furniture : tile->GetFurnitures())
		furniture_list_pane.AddPane(new ObjectIconButton(furniture, this));
	furniture_list_pane.ResizePanes(3);
}

void MapEditor::TileDetailPane::ActorClicked(Actor *actor_object)
{
	GetDetailPaneBrowser()->Open(new ObjectDetailPane(actor_object));
}

void MapEditor::TileDetailPane::ItemClicked(Item *item_object)
{
	GetDetailPaneBrowser()->Open(new ObjectDetailPane(item_object));
}

void MapEditor::TileDetailPane::FurnitureClicked(Furniture *furniture_object)
{
	GetDetailPaneBrowser()->Open(new ObjectDetailPane(furniture_object));
}

void MapEditor::TileDetailPane::ObjectClicked(void *tile_detail_pane_, Object *object_)
{
	TileDetailPane *tile_detail_pane= static_cast<TileDetailPane *>(tile_detail_pane_);

	if(Utility::IsType<Actor>(object_))
		tile_detail_pane->ActorClicked(dynamic_cast<Actor *>(object_));
	else if(Utility::IsType<Item>(object_))
		tile_detail_pane->ItemClicked(dynamic_cast<Item *>(object_));
	else if(Utility::IsType<Furniture>(object_))
		tile_detail_pane->FurnitureClicked(dynamic_cast<Furniture *>(object_));
}

void MapEditor::TileDetailPane::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		ButtonInterfaceEvent *button_interface_event= dynamic_cast<ButtonInterfaceEvent *>(interface_event);

		GetDetailPaneBrowser()->Open(new ObjectDetailPane(dynamic_cast<ObjectIconButton *>(button_interface_event->GetButton())->GetObject()));
	}
}

MapEditor::TileDetailPane::TileDetailPane(Tile *tile_)
	: actor_icon_button(tile_->GetActor(), this), 
	  background("nine.png", Ninepatch::Roundness::Normal), 
	  item_list_pane(ListPane::Direction::LeftToRight), 
	  furniture_list_pane(ListPane::Direction::LeftToRight)
{
	AcceptParcels();

	tile= tile_;

	Size= Vec2f(1.0f, 0.35f);
	Offset= Vec2f(0.0f, 0.645f);//***Figure out way to make TopToBottom list panes render first element level with this without finicking with offset 
	//***Even better if you can make panes render top to bottom so we don't have to manually compute offsets given size

	actor_icon_button.Size= Vec2f(0.4f, 0.333f);
	actor_icon_button.Offset= Vec2f(0.0f, 0.666f);

	item_list_pane.Size= Vec2f(1.0f, 0.333f);
	item_list_pane.Offset= Vec2f(0.0f, 0.333f);
	item_list_pane.FrameContent();

	furniture_list_pane.Size= Vec2f(1.0f, 0.333f);
	furniture_list_pane.Offset= Vec2f(0.0f, 0.0f);
	furniture_list_pane.FrameContent();

	FillListPanes();

	AddComponent(&background);
	AddComponent(&actor_icon_button);
	AddComponent(&item_list_pane);
	AddComponent(&furniture_list_pane);
}

void MapEditor::TileDetailPane::RecieveParcel(Parcel * parcel)
{
	DontRebroadcastCurrentEvent();

	if(!Utility::IsType<ObjectParcel>(parcel))
		return;

	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);
	if(object_parcel->IsItemOfType<Actor>())
	{
		if(tile->HasActor())
			return;

		Actor *actor= dynamic_cast<Actor *>(object_parcel->SignFor());

		tile->PutActor(actor);

		actor_icon_button.SetObject(actor);
	}
	else if(object_parcel->IsItemOfType<Item>())
	{
		Item *item= dynamic_cast<Item *>(object_parcel->SignFor());

		tile->PutItem(item);

		FillListPanes();
	}
	else if(object_parcel->IsItemOfType<Furniture>())
	{
		Furniture *furniture= dynamic_cast<Furniture *>(object_parcel->SignFor());

		tile->PutFurniture(furniture);

		FillListPanes();
	}

	DontRebroadcastCurrentEvent();
}

void MapEditor::TileDetailPane::RelinquishParcel(Parcel *parcel, bool must_transfer_ownership)
{
	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);

	if(object_parcel->IsItemOfType<Actor>())
	{
		tile->RemoveActor();
		actor_icon_button.SetObject(nullptr);
	}
	else if(object_parcel->IsItemOfType<Item>())
	{
		Item *item= nullptr;
		for(ObjectIconButton *object_icon_button : item_list_pane.GetPanesOfType<ObjectIconButton>())
			if(object_parcel->IsThisTheItem(object_icon_button->GetObject()))
				item= dynamic_cast<Item *>(object_icon_button->GetObject());

		tile->RemoveItem(item);
		FillListPanes();
	}
	else if(object_parcel->IsItemOfType<Furniture>())
	{
		Furniture *furniture= nullptr;
		for(ObjectIconButton *object_icon_button : furniture_list_pane.GetPanesOfType<ObjectIconButton>())
			if(object_parcel->IsThisTheItem(object_icon_button->GetObject()))
				furniture= dynamic_cast<Furniture *>(object_icon_button->GetObject());

		tile->RemoveFurniture(furniture);
		FillListPanes();
	}
}


void MapEditor::ObjectDetailPane::EntityElement::MeshPane::FillElementNameDropdown()
{
	element_name_dropdown.ClearElements();

	std::map<string, Mesh *> mesh_map= entity_element->entity->GetMeshes();
	vector<Mesh *> meshes= Mesh::Parse(Utility::GetFilename(mesh_map[name_textbox.GetText()]->GetFilepath()));
	for(Mesh *mesh : meshes)
		element_name_dropdown.AddElement(mesh->GetElementPath()[0]);

	element_name_dropdown.SetSelectedElement(mesh_map[name_textbox.GetText()]->GetElementPath()[0]);
}

void MapEditor::ObjectDetailPane::EntityElement::MeshPane::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<SelectFileButtonInterfaceEvent>(interface_event))
	{
		SelectFileButtonInterfaceEvent * select_file_button_interface_event= dynamic_cast<SelectFileButtonInterfaceEvent *>(interface_event);

		Mesh *mesh= Mesh::Parse(Utility::GetFilename(select_file_button_interface_event->GetFilepath()))[0]; 
		string text= name_textbox.GetText();
		entity_element->entity->SetMesh(text, mesh);

		FillElementNameDropdown();
	}
	else if(Utility::IsType<DropdownInterfaceEvent>(interface_event))
	{
		DropdownInterfaceEvent * dropdown_interface_event= dynamic_cast<DropdownInterfaceEvent *>(interface_event);	

		Mesh *mesh= Mesh::Retrieve(filename_button.GetFilename(), dropdown_interface_event->GetSelectedElement());
		string text= name_textbox.GetText();

		entity_element->entity->SetMesh(text, mesh);
	}
}

MapEditor::ObjectDetailPane::EntityElement::MeshPane::MeshPane(EntityElement *entity_element_, string name)
	: ListPane(Direction::TopToBottom), name_textbox(name), filename_button("blank", Utility::MakeVector<string>("obj"))
{
	entity_element= entity_element_;

	filename_button.SetFilepath(entity_element->entity->GetMeshes()[name]->GetFilepath());

	FillElementNameDropdown();

	AddPane(&name_textbox);
	AddPane(&filename_button);
	AddPane(&element_name_dropdown);

	ResizePanes();
}


MapEditor::ObjectDetailPane::EntityElement::EntityElement(Entity *entity_)
	: mesh_list_pane(ListPane::Direction::LeftToRight)
{
	entity= entity_;

	Size= Vec2f(1.0f, 0.3f);

	mesh_list_pane.Size= Vec2f(1.0f, 1.0f);

	std::map<string, Mesh *> mesh_map= entity->GetMeshes();
	for(std::pair<string, Mesh *> mesh_pair : mesh_map)
		mesh_list_pane.AddPane(new MeshPane(this, mesh_pair.first));

	AddComponent(&mesh_list_pane);
}


void MapEditor::ObjectDetailPane::ActorElement::FillInventoryPane()
{
	inventory_list_pane.ClearPanes(true);

	for(Item *item : actor->GetInventory())
		inventory_list_pane.AddPane(new ObjectIconButton(item, this));
	inventory_list_pane.ResizePanes(3);
}

void MapEditor::ObjectDetailPane::ActorElement::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		ButtonInterfaceEvent *button_interface_event= dynamic_cast<ButtonInterfaceEvent *>(interface_event);

		object_detail_pane->GetDetailPaneBrowser()->Open(new ObjectDetailPane(dynamic_cast<ObjectIconButton *>(button_interface_event->GetButton())->GetObject()));
	}
}

MapEditor::ObjectDetailPane::ActorElement::ActorElement(Actor *actor_, ObjectDetailPane *object_detail_pane_)
	: inventory_list_pane(ListPane::Direction::LeftToRight, 1, true)
{
	actor= actor_;

	Size= Vec2f(1.0f, 0.1f);
	AcceptParcels();

	FillInventoryPane();

	inventory_list_pane.Size= Vec2f(0.9f, 0.9f);
	inventory_list_pane.Offset= Vec2f(0.05f, 0.05f);
	AddComponent(&inventory_list_pane);

	object_detail_pane= object_detail_pane_;
}

void MapEditor::ObjectDetailPane::ActorElement::RecieveParcel(Parcel *parcel)
{
	if(!Utility::IsType<ObjectParcel>(parcel))
		return;

	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);
	if(!object_parcel->IsItemOfType<Item>())
		return;

	Item *item= dynamic_cast<Item *>(object_parcel->SignFor());

	actor->GiveItem(item);

	FillInventoryPane();

	DontRebroadcastCurrentEvent();
}

void MapEditor::ObjectDetailPane::ActorElement::RelinquishParcel(Parcel *parcel, bool must_transfer_ownership)
{
	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);

	Item *item= nullptr;
	for(ObjectIconButton *object_icon_button : inventory_list_pane.GetPanesOfType<ObjectIconButton>())
		if(object_parcel->IsThisTheItem(object_icon_button->GetObject()))
			item= dynamic_cast<Item *>(object_icon_button->GetObject());

	assert(item!= nullptr && "MapEditor::ObjectDetailPane::ActorElement::RelinquishParcel(): Parcel Object not found.");
	if(item== nullptr)
		return;

	actor->TakeItem(item);
	FillInventoryPane();
}


MapEditor::ObjectDetailPane::ItemElement::ItemElement(Item *item)
{
	Size= Vec2f(1.0f, 0.1f);
}


Direction::Enum MapEditor::ObjectDetailPane::FurnitureElement::GetDirection(ToggleButton *direction_button)
{
	int button_index= -1;
	for(int i= 0; i< 8; i++)
		if(direction_button== direction_buttons+ i)
			button_index= i;

	switch(button_index)
	{
	case 0: return Direction::Southwest;
	case 1: return Direction::West;
	case 2: return Direction::Northwest;
	case 3: return Direction::North;
	case 4: return Direction::Northeast;
	case 5: return Direction::East;
	case 6: return Direction::Southeast;
	case 7: return Direction::South;

	default:
		assert(false && "MapEditor::ObjectDetailPane::FurnitureElement::GetDirection(): ToggleButton passed not found in direction_buttons array.");
		return (Direction::Enum)0;
	}
}

void MapEditor::ObjectDetailPane::FurnitureElement::UpdateFurnitureObject()
{
	furniture->ClearBlockedDirections();

	for(int i= 0; i< 8; i++)
		if(!direction_buttons[i].IsOn())
			furniture->AddBlockedDirection(GetDirection(direction_buttons+ i));
}

void MapEditor::ObjectDetailPane::FurnitureElement::UpdateDirectionButtonState()
{
	for(int i= 0; i< 8; i++)
	{
		if(furniture->IsDirectionBlocked(GetDirection(direction_buttons+ i)))
			direction_buttons[i].TurnOff();
		else
			direction_buttons[i].TurnOn();
	}
}

void MapEditor::ObjectDetailPane::FurnitureElement::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		UpdateFurnitureObject();
	}
}

MapEditor::ObjectDetailPane::FurnitureElement::FurnitureElement(Furniture *furniture_)
{
	furniture= furniture_;

	Size= Vec2f(1.0f, 0.3f);

	for(int i= 0; i< 8; i++)
	{
		direction_buttons[i].Size= Vec2f(0.333f, 0.333f);
		AddComponent(&direction_buttons[i]);
	}

	direction_buttons[0].Offset= Vec2f(0.000f, 0.666f);
	direction_buttons[1].Offset= Vec2f(0.000f, 0.333f);
	direction_buttons[2].Offset= Vec2f(0.000f, 0.000f);
	direction_buttons[3].Offset= Vec2f(0.333f, 0.000f);
	direction_buttons[4].Offset= Vec2f(0.666f, 0.000f);
	direction_buttons[5].Offset= Vec2f(0.666f, 0.333f);
	direction_buttons[6].Offset= Vec2f(0.666f, 0.666f);
	direction_buttons[7].Offset= Vec2f(0.333f, 0.666f);
	
	UpdateDirectionButtonState();
}

void MapEditor::ObjectDetailPane::FurnitureElement::KeyUp(ButtonType button)
{
	int rotation= 0;

	if(button== ButtonType::Z)
		rotation= 2;
	else if(button== ButtonType::X)
		rotation= 6;

	if(rotation!= 0)
	{
		vector<Direction::Enum> blocked_directions= furniture->GetBlockedDirections();
		furniture->ClearBlockedDirections();

		for(Direction::Enum direction : blocked_directions)
			furniture->AddBlockedDirection((Direction::Enum)(((direction- Direction::East)+ rotation)% 8+ Direction::East));

		UpdateDirectionButtonState();
	}
}


MapEditor::ObjectDetailPane::ObjectDetailPane(Object *object_)
	: element_list_pane(ListPane::Direction::TopToBottom)
{
	this->object_= object_;

	element_list_pane.AddPane(new EntityElement(object_));

	if(Utility::IsType<Actor>(object_))
		element_list_pane.AddPane(new ActorElement(dynamic_cast<Actor *>(object_), this));
	else if(Utility::IsType<Item>(object_))
		element_list_pane.AddPane(new ItemElement(dynamic_cast<Item *>(object_)));
	else if(Utility::IsType<Furniture>(object_))
		element_list_pane.AddPane(new FurnitureElement(dynamic_cast<Furniture *>(object_)));

	AddComponent(&element_list_pane);
}


void MapEditor::Toolbox::ObjectDragged(Object *object_)
{
	Object *object_copy= Encodable::Copy(object_);
	SendParcel(new MouseFollowingParcel<Object>(this, object_copy, Vec2f(0.15f, 0.15f), new IconPane(object_copy->GetIconMesh(), 2)));
}

void MapEditor::Toolbox::HandleInterfaceEvent(InterfaceEvent * interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event))
	{
		ButtonInterfaceEvent * button_interface_event= dynamic_cast<ButtonInterfaceEvent *>(interface_event);	

		if(Utility::IsType<ObjectIconButton>(button_interface_event->GetButton()))
			SetActiveObject(dynamic_cast<ObjectIconButton *>(button_interface_event->GetButton())->GetObject());
		else
			detail_pane_browser->Open(new NewObjectDetailPane(this));
	}
}

MapEditor::Toolbox::Toolbox(string filepath, DetailPaneBrowser *detail_pane_browser_)
	: GridPane(3, 10), 
	  new_object_button("nine.png", Ninepatch::Roundness::Normal, new ImagePane("new.png"))
{
	AddPane(&new_object_button);

	Load(filepath);

	detail_pane_browser= detail_pane_browser_;

	AcceptParcels();
}

void MapEditor::Toolbox::Save(string filepath)
{
	Savable::Save(filepath);

	TiXmlDocument xml_document;
	xml_document.LoadFile(filepath.c_str());

	TiXmlElement *xml_toolbox= new TiXmlElement("toolbox");

	for(ObjectIconButton *object_icon_button : GetPanesOfType<ObjectIconButton>())
	{
		TiXmlElement *xml_object= object_icon_button->GetObject()->EncodeXML();
		xml_object->LinkEndChild(Utility::XML::MakeElementWithText("scale", std::to_string(1)));

		xml_toolbox->LinkEndChild(xml_object);
	}

	xml_document.LinkEndChild(xml_toolbox);
	xml_document.SaveFile(filepath.c_str());
}

void MapEditor::Toolbox::Load(string filepath)
{
	Savable::Load(filepath);

	ClearPanes();

	TiXmlDocument xml_document;
	xml_document.LoadFile(filepath.c_str());

	TiXmlElement *xml_toolbox= xml_document.FirstChildElement("toolbox");

	TiXmlElement *xml_object= xml_toolbox->FirstChildElement();
	while(xml_object!= nullptr)
	{
		float scale= std::stof(xml_object->FirstChildElement("scale")->GetText());

		AddObject(Encodable::DecodeXML<Object>(xml_object));

		xml_object= xml_object->NextSiblingElement();
	}
}

void MapEditor::Toolbox::AddObject(Object *object_)
{
	RemovePane(&new_object_button);
	AddPane(new ObjectIconButton(object_, this));
	AddPane(&new_object_button);

	SetActiveObject(nullptr);
}

Object * MapEditor::Toolbox::GetActiveObject()
{
	return active_object;
}

void MapEditor::Toolbox::SetActiveObject(Object *object_)
{
	if(object_== nullptr)
	{
		Deselect();
		active_object= object_;
		return;
	}

	active_object= object_;
	for(ObjectIconButton *object_icon_button : GetPanesOfType<ObjectIconButton>())
		if(object_icon_button->GetObject()== object_)
			Select(object_icon_button);

	this->detail_pane_browser->Open(new ObjectDetailPane(object_));
}

void MapEditor::Toolbox::RecieveParcel(Parcel *parcel)
{
	if(!Utility::IsType<ObjectParcel>(parcel))
		return;

	AddObject(dynamic_cast<ObjectParcel *>(parcel)->SignFor(true));

	DontRebroadcastCurrentEvent();
}

void MapEditor::Toolbox::RelinquishParcel(Parcel *parcel, bool must_transfer_ownership)
{
	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);

	ObjectIconButton *source_object_icon_button= nullptr;
	for(ObjectIconButton *object_icon_button : GetPanesOfType<ObjectIconButton>())
		if(object_parcel->IsThisTheItem(object_icon_button->GetObject()))
			source_object_icon_button= object_icon_button;

	assert(source_object_icon_button!= nullptr && "MapEditor::Toolbox::RelinquishParcel(): Parcel Object not found.");
	if(source_object_icon_button== nullptr)
		return;

	if(must_transfer_ownership)
		RemovePane(source_object_icon_button, true);
	else
		source_object_icon_button->SetObject(Encodable::Copy(source_object_icon_button->GetObject()));
}

void MapEditor::Toolbox::Draw()
{
	ListPane::Draw();
}

/*
void MapEditor::Toolbox::ObjectClicked_Callback(void *toolbox_instance, Object *object_)
{
	static_cast<Toolbox *>(toolbox_instance)->SetActiveObject(object_);
}

void MapEditor::Toolbox::NewButtonClicked_Callback(void *toolbox_instance, Button *new_object_button)
{
	Toolbox *toolbox= dynamic_cast<Toolbox *>(static_cast<Interface *>(toolbox_instance));

	toolbox->detail_pane_browser->Open(new NewObjectDetailPane(toolbox));
}
*/

void MapEditor::NewObjectDetailPane::HandleInterfaceEvent(InterfaceEvent * interface_event)
{
	if(Utility::IsType<DropdownInterfaceEvent>(interface_event))
	{
		DropdownInterfaceEvent * dropdown_interface_event= dynamic_cast<DropdownInterfaceEvent *>(interface_event);	

		string object_type_string= dropdown_interface_event->GetSelectedElement();

		Object *object_= nullptr;
		if(object_type_string== "Actor")
			object_= new Actor();
		else if(object_type_string== "Item")
			object_= new Item();
		else if(object_type_string== "Gun")
			object_= new Gun();
		else if(object_type_string== "Furniture")
			object_= new Furniture();

		object_->SetMesh("default", Mesh::Retrieve("suzanne.obj", "Suzanne"));

		toolbox->AddObject(object_);
		toolbox->SetActiveObject(object_);
	}
}

MapEditor::NewObjectDetailPane::NewObjectDetailPane(Toolbox *toolbox_)
{
	toolbox= toolbox_;

	object_type_dropdown.Size= Vec2f(1.0f, 0.1f);
	object_type_dropdown.Offset= Vec2f(0.0f, 0.9f);
	object_type_dropdown.AddElements(Utility::MakeVector<string>("Actor", "Item", "Gun", "Furniture"));

	AddComponent(&object_type_dropdown);
}


void MapEditor::TrashPane::AddObject(Object *object_)
{
	list_pane.AddPane(new ObjectIconButton(object_, this));
}

MapEditor::TrashPane::TrashPane()
	: trash_button("trashcan.png"), list_pane(ListPane::Direction::BottomToTop)
{
	list_pane.Offset= Vec2f(0.0f, 1.0f);
	list_pane.Hide();
	list_pane.DontIntersectParentBounds();
	AddComponent(&list_pane);

	AddComponent(&trash_button);

	AcceptParcels();
	DontIntersectParentBounds();
}

void MapEditor::TrashPane::RecieveParcel(Parcel *parcel)
{
	if(!Utility::IsType<ObjectParcel>(parcel))
		return;

	AddObject(dynamic_cast<ObjectParcel *>(parcel)->SignFor(true));

	DontRebroadcastCurrentEvent();
}

void MapEditor::TrashPane::RelinquishParcel(Parcel *parcel, bool must_transfer_ownership)
{
	ObjectParcel *object_parcel= dynamic_cast<ObjectParcel *>(parcel);

	ObjectIconButton *source_object_icon_button= nullptr;
	for(Pane *pane : list_pane.GetPanes())
	{
		ObjectIconButton *object_icon_button= dynamic_cast<ObjectIconButton *>(pane);

		if(object_parcel->IsThisTheItem(object_icon_button->GetObject()))
			source_object_icon_button= object_icon_button;
	}

	assert(source_object_icon_button!= nullptr && "MapEditor::TrashPane::RelinquishParcel(): Parcel Object not found.");
	if(source_object_icon_button== nullptr)
		return;

	list_pane.RemovePane(source_object_icon_button, true);
}

void MapEditor::TrashPane::Focus()
{
	Pane::Focus();

	list_pane.Show();
}

void MapEditor::TrashPane::Defocus()
{
	Pane::Defocus();

	list_pane.Hide();
}

void MapEditor::TrashPane::MouseLeftClick()
{
	if(IsFocused())
		Defocus();
	else
		Focus();
}


bool MapEditor::PutObjectIntoTile(Object *object_, Tile *tile)
{
	if(Utility::IsType<Actor>(object_) && !tile->HasActor())
	{ 
		tile->PutActor(dynamic_cast<Actor *>(object_));
		return true;
	}
	else if(Utility::IsType<Item>(object_))
	{
		tile->PutItem(dynamic_cast<Item *>(object_));
		return true;
	}
	else if(Utility::IsType<Furniture>(object_))
	{
		tile->PutFurniture(dynamic_cast<Furniture *>(object_));
		return true;
	}

	return false;
}

void MapEditor::RemoveObjectFromTile(Object *object_, Tile *tile)
{
	if(Utility::IsType<Actor>(object_) && tile->HasActor())
		tile->RemoveActor();
	else if(Utility::IsType<Item>(object_))
		tile->RemoveItem(dynamic_cast<Item *>(object_));
	else if(Utility::IsType<Furniture>(object_))
		tile->RemoveFurniture(dynamic_cast<Furniture *>(object_));
}

void MapEditor::ClearPreviewObject()
{
	if(preview_object== nullptr)
		return;

	RemoveObjectFromTile(preview_object, preview_tile);

	preview_object= nullptr;
	preview_tile= nullptr;
}

void MapEditor::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<_3DInterfacePane::TileSelectedEvent>(interface_event))
	{
		_3DInterfacePane::TileSelectedEvent *tile_selected_event= dynamic_cast<_3DInterfacePane::TileSelectedEvent *>(interface_event);

		Tile *selected_tile= tile_selected_event->GetSelectedTile();

		ClearPreviewObject();

		if(toolbox.GetActiveObject()!= nullptr)
		{
			Object *active_object_copy= Encodable::Copy(toolbox.GetActiveObject());
			PutObjectIntoTile(active_object_copy, selected_tile);
		}

		detail_pane_browser.Open(new TileDetailPane(selected_tile));

		return;
	}
	else if(Utility::IsType<_3DInterfacePane::TileHoveredEvent>(interface_event))
	{
		_3DInterfacePane::TileHoveredEvent *tile_hovered_event= dynamic_cast<_3DInterfacePane::TileHoveredEvent *>(interface_event);

		Tile *hovered_tile= tile_hovered_event->GetHoveredTile();

		ClearPreviewObject();

		if(toolbox.GetActiveObject()!= nullptr)
		{
			Object *active_object= toolbox.GetActiveObject();

			if(PutObjectIntoTile(active_object, hovered_tile))
			{
				preview_tile= hovered_tile;
				preview_object= active_object;
			}
		}

		return;
	}
	

	Pane::HandleInterfaceEvent(interface_event);
}

MapEditor::MapEditor(Camera *camera)
	: _3d_interface_pane(camera), toolbox(System::game.space.GetFilepath(), &detail_pane_browser)
{
	detail_pane_browser.Size= Vec2f(0.25f, 0.9f);
	detail_pane_browser.Offset= Vec2f(0.3f, 0.05f);

	toolbox.Size= Vec2f(0.27f, 0.9f);
	toolbox.Offset= Vec2f(0.01f, 0.05f);

	trash_pane.Size= Vec2f(0.1f, 0.1f);
	trash_pane.Offset= Vec2f(0.9f, 0.0f);

	AddComponent(&_3d_interface_pane);
	AddComponent(&detail_pane_browser);
	AddComponent(&toolbox);
	AddComponent(&trash_pane);
}

void MapEditor::SetScene(Scene *scene)
{
	_3d_interface_pane.SetScene(scene);
}

void MapEditor::SaveToolbox(string filepath)
{
	toolbox.Save(filepath);
}

void MapEditor::LoadToolbox(string filepath)
{
	toolbox.Load(filepath);
}

void MapEditor::KeyUp(ButtonType button)
{
	Pane::KeyUp(button);

	if(button== ButtonType::Backspace)
		detail_pane_browser.Back();
}

void MapEditor::MouseRightClick()
{
	if(!is_rotating_preview)
	{
		toolbox.SetActiveObject(nullptr);
		ClearPreviewObject();
	}
	else
	{
		is_rotating_preview= false;
		_3d_interface_pane.UnlockSelection();

		System::graphics.MoveMouseTo(mouse_position_before_preview_rotation);
	}
}

void MapEditor::MouseRightDown(Vec2f mouse_position)
{
	preview_rotation= preview_object!= nullptr ? preview_object->GetRotation() : 0;
}

void MapEditor::MouseRightDrag(Vec2f mouse_position, Vec2f mouse_displacement)
{
	Object *object_= toolbox.GetActiveObject();
	if(object_== nullptr)
		return;

	if(!is_rotating_preview)
	{
		mouse_position_before_preview_rotation= mouse_position;
		is_rotating_preview= true;
	}

	_3d_interface_pane.LockSelection();

	preview_rotation+= Math::DegreesToRadians(30* 100* mouse_displacement.x);
	
	if(System::input.IsKeyDown(ButtonEvent::Button::Ctrl))
		object_->SetRotation((int)(preview_rotation/ Math::DegreesToRadians(45))* Math::DegreesToRadians(45));
	else
		object_->SetRotation(preview_rotation);
}

void MapEditor::Draw()
{
	_3d_interface_pane.ClearHighlightedTiles();

	if(Utility::IsType<Furniture>(toolbox.GetActiveObject()))
	{
		Furniture *furniture= dynamic_cast<Furniture *>(toolbox.GetActiveObject());

		for(unsigned int i= Direction::East; i<= Direction::Southeast; i++)
		{
			Direction::Enum direction= (Direction::Enum)i;

			if(!furniture->IsDirectionBlocked(direction))
			{
				Tile *destination= Direction::GetDestination(_3d_interface_pane.GetHoveredTile(), direction);

				if(destination!= nullptr)
					_3d_interface_pane.AddHighlightedTile(destination, Color::Mix(Color::Green, Color::Yellow, 0.5f));
			}
		}
	}

	Pane::Draw();
}

void MapEditor::HandleEvent(Event *event_)
{
	Pane::HandleEvent(event_);

	if(Interface::IsParcelActive())
		_3d_interface_pane.Deactivate();
	else
		_3d_interface_pane.Activate();
}

void DevelopmentInterface::HandleInterfaceEvent(InterfaceEvent *interface_event)
{
	if(Utility::IsType<ButtonInterfaceEvent>(interface_event) && dynamic_cast<ButtonInterfaceEvent *>(interface_event)->GetButton()== &mode_toggle_button)
	{
		if(tactical_interface_is_active)
		{
			mode_toggle_button.SetText(map_editor_toggle_text);
			tactical_interface.Hide();
			map_editor.Show();
		}
		else
		{
			mode_toggle_button.SetText(tactical_interface_toggle_text);
			map_editor.Hide();
			tactical_interface.Show();
		}

		tactical_interface_is_active= !tactical_interface_is_active;

		Menu::Back();
	}
	else if(Utility::IsType<SaveFileDialog::FilepathSelectedEvent>(interface_event))
	{
		string filepath= dynamic_cast<SaveFileDialog::FilepathSelectedEvent *>(interface_event)->GetFilepath();

		System::game.space.Save(filepath);
		map_editor.SaveToolbox(filepath);
		Menu::pause_menu->SetSaveLoadFilepath(filepath);

		return;
	}
	else if(Utility::IsType<LoadFileDialog::FilepathSelectedEvent>(interface_event))
	{
		string filepath= dynamic_cast<LoadFileDialog::FilepathSelectedEvent *>(interface_event)->GetFilepath();

		System::game.space.Load(filepath);
		map_editor.LoadToolbox(filepath);
		Menu::pause_menu->SetSaveLoadFilepath(filepath);

		return;
	}
}

DevelopmentInterface::DevelopmentInterface()
	: camera(40, Vec3f(2, 1.5, 2), Math::DegreesToRadians(-15), Math::DegreesToRadians(80)),
	  tactical_interface(&camera), 
	  map_editor(&camera), 
	  mode_toggle_button("nine.png", "", Color::Black)
{
	scene.AddProp(&System::game.space);
	scene.AddLight(new PointLight(Vec3f(16.0f, 25.0f, 10.0f), Color(0.92f, 0.85f, 0.77f)));
	scene.Commit();
	tactical_interface.SetScene(&scene);
	map_editor.SetScene(&scene);

	tactical_interface.Hide();

	AddComponent(&tactical_interface);
	AddComponent(&map_editor);

	mode_toggle_button.SetText(map_editor_toggle_text);
	mode_toggle_button.AddInterfaceEventRecipient(this);
	Menu::InitializeMenus();
	Menu::pause_menu->AddButton(&mode_toggle_button);
	Menu::pause_menu->AddInterfaceEventRecipient(this);
	Menu::pause_menu->SetSaveLoadFilepath("data/content/maps/map.xml");
}

void DevelopmentInterface::KeyUp(ButtonType button)
{
	if(button== ButtonType::Esc)
		Menu::pause_menu->Open();
}
