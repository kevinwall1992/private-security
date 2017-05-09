#include "UI.h"
#include "InputSystem.h"
#include "GameSystem.h"
#include "Viewport.h"
#include "Pathing.h"
#include "Actions.h"


AboutDialog::AboutDialog()
	: Dialog("nine.png", "Private Security\nv"+ Utility::TextFileToString("data/other/version.txt"))
{
	Size= Vec2f(0.3f, 0.2f);
	Offset= Vec2f(0.35f, 0.4f);
}

AboutButton::AboutButton()
	: OpenButton("nine.png", "About", Color::Black, Menu::about_dialog)
{
	
}

QuitButton::QuitButton()
	: TextButton("nine.png", "Quit", Color::Black)
{

}

void QuitButton::MouseLeftClick()
{
	System::input.Quit();
}

ResumeButton::ResumeButton()
	: BackButton("nine.png", "Resume", Color::Black)
{

}

SaveButton::SaveButton()
	: TextButton("nine.png", "Save", Color::Black)
{
}

void SaveButton::MouseLeftClick()
{
	System::game.Save("default.save");
}

LoadButton::LoadButton()
	: TextButton("nine.png", "Load", Color::Black)
{
	
}

void LoadButton::MouseLeftClick()
{
	System::game.Load("default.save");
}


PauseMenu::PauseMenu()
	: button_list_pane(ButtonListPane::Direction::TopToBottom, 5)
{
	button_list_pane.AddButton(&about_button);
	button_list_pane.AddButton(&save_button);
	button_list_pane.AddButton(&load_button);
	button_list_pane.AddButton(&resume_button);
	button_list_pane.AddButton(&quit_button);

	AddComponent(&button_list_pane);

	Size= Vec2f(0.25f, 0.4f);
	Offset= Vec2f(0.375f, 0.45f);

	button_group.AddTextPane(about_button.GetTextPane());
	button_group.AddTextPane(save_button.GetTextPane());
	button_group.AddTextPane(load_button.GetTextPane());
	button_group.AddTextPane(resume_button.GetTextPane());
	button_group.AddTextPane(quit_button.GetTextPane());
}


IconCamera * IconPane::icon_camera= nullptr;

IconPane::IconPane(Mesh *mesh_)
	: cycle(4.0f, 0.5f)
{
	scene.AddLight(new PointLight(Vec3f(3.0f, 10.0f, 5.0f), Color::White));
	scene.AddProp(new MeshProp(mesh_));
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
	if(icon_camera== nullptr)
		icon_camera= new IconCamera(Math::DegreesToRadians(35), Vec3f(3, 3, 5), Vec3f(0, 0.5f, 0));

	dynamic_cast<MeshProp *>(scene.GetProps()[0])->SetRotation((float)(2* M_PI* cycle.GetMoment()));

	Viewport::Push();
	Framebuffer framebuffer= Framebuffer::GetCurrentFramebuffer();

	int width= System::graphics.GetScreenSize().x/ 5;
	Texture texture= icon_camera->TakePhoto(scene, Vec2i(width, width), Photo::Type::FullColor).GetTexture();

	framebuffer.Bind();
	Viewport::Pop();

	return texture;
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
	: ButtonListPane(Direction::LeftToRight, 10)
{
	for(unsigned int i= 0; i< 10; i++)
		AddButton(slots+ i);

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

	ButtonListPane::Draw();
}


float TacticalInterface::tile_frame_vertices[24* 3]= { -0.5f, 0.0f, -0.5f,  +0.5f, 0.0f, -0.5f, 
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

Tile * TacticalInterface::GetTile(Ray ray)
{
	Vec3f normal= Vec3f(0, 1, 0);
	Vec3i intersection_point= ray.tail+ ray.direction* (normal* (float)level- ray.tail).Dot(normal)/ ray.direction.Dot(normal);
	intersection_point.y= level;

	return System::game.space.GetTile(intersection_point);
}

void TacticalInterface::HandleEvent(Event *event_)
{
	CameraPane::HandleEvent(event_);

	Menu::HandleEvent_Menus(event_);
}

TacticalInterface::TacticalInterface(Camera *camera)
	: CameraPane(camera)
{
	GLuint tile_frame_vertex_buffer_handle;
	glGenBuffers(1, &tile_frame_vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, tile_frame_vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, 24* 3* sizeof(float), tile_frame_vertices, GL_STATIC_DRAW);
	tile_frame_vao.SetAttributeBinding3f(ShaderProgram::Retrieve("lines.program")->GetAttributeLocation("position"));

	camera_gizmo= new CameraGizmo(camera);
	AddComponent(camera_gizmo);

	AddComponent(&toolbelt);

	Menu::InitializeMenus();
	Menu::SetHost(this);
}

void TacticalInterface::KeyUp(ButtonType button)
{
	if(button== ButtonType::Esc)
		Menu::pause_menu->Open();
}

void TacticalInterface::MouseLeftUpRay(Ray ray)
{
	Tile *tile= GetTile(ray);
	if(tile!= nullptr && tile->GetActor()!= nullptr)
	{
		selected_actor= tile->GetActor();
		toolbelt.SetActor(selected_actor);
	}
}

void TacticalInterface::MouseRightUpRay(Ray ray)
{
	Tile *tile= GetTile(ray);

	if(tile!= nullptr && selected_actor!= nullptr && selected_actor->GetAction()== nullptr)
	{
		Path path= Path::GetPath(new Node(selected_actor, selected_actor->GetTile()), new Node(selected_actor, tile));

		Edge *edge;
		while((edge= path.PopEdge())!= nullptr)
			System::game.SubmitAction(new MoveAction(selected_actor, edge->GetMove()));
	}
}

void TacticalInterface::MouseMotionRay(Ray ray, Ray last_ray)
{
	hovered_tile= GetTile(ray);
}

void TacticalInterface::MouseScroll(int scroll_amount)
{
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

void TacticalInterface::MouseIn()
{
	
}

void TacticalInterface::MouseOut()
{
	
}

void TacticalInterface::Draw()
{
	CameraPane::Draw();

	if(hovered_tile!= nullptr)
	{
		Vec3f tile_position= hovered_tile->GetPosition();

		ShaderProgram *shader_program= ShaderProgram::Retrieve("lines.program");
		shader_program->Use();
		shader_program->SetUniformMatrix4x4f("model_transform", Transform().Translate(Vec3f(tile_position.x, tile_position.y, tile_position.z)+ Vec3f(0.5f, 0.0f, 0.5f)).GetMatrix());
		shader_program->SetUniformMatrix4x4f("camera_transform", GetCamera()->GetProjectedTransform(Math::GetAspectRatio(GetPixelSize())).GetMatrix());
		if(IsHovered())
			shader_program->SetUniformVector3f("line_color", Vec3f(0, 1, 0));
		else
			shader_program->SetUniformVector3f("line_color", Vec3f(1, 0, 0));

		AttachDepth();
		Viewport::Push(GetPixelOffset(), GetPixelSize());
		tile_frame_vao.Bind();
		glDrawArrays(GL_LINES, 0, 24);
		Viewport::Pop();
	}

	Menu::Draw_Menus();
}
