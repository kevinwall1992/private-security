#include "UI.h"
#include "InputSystem.h"
#include "GameSystem.h"
#include "Viewport.h"


float TacticalPane::tile_frame_vertices[24* 3]= { -0.5f, 0.0f, -0.5f,  +0.5f, 0.0f, -0.5f, 
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

Tile * TacticalPane::GetTile(Ray ray)
{
	Vec3f normal= Vec3f(0, 1, 0);
	Vec3i intersection_point= ray.tail+ ray.direction* (normal* (float)level- ray.tail).Dot(normal)/ ray.direction.Dot(normal)+ Vec3f(0.5f, 0.0f, 0.5f);

	return System::game.space.GetTile(intersection_point);
}

void TacticalPane::HandleEvent(Event *event_)
{
	CameraPane::HandleEvent(event_);

	camera_gizmo->HandleEvent(event_);
}

TacticalPane::TacticalPane(Camera *camera)
	: CameraPane(camera), button("nine.png")
{
	GLuint tile_frame_vertex_buffer_handle;
	glGenBuffers(1, &tile_frame_vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, tile_frame_vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, 24* 3* sizeof(float), tile_frame_vertices, GL_STATIC_DRAW);
	tile_frame_vao.SetAttributeBinding3f(ShaderProgram::Retrieve("lines.program")->GetAttributeLocation("position"));

	camera_gizmo= new CameraGizmo(camera);

	button.SetParent(this);
	button.Offset= Vec2f(0.25f, 0.25f);
	button.Size= Vec2f(0.1f, 0.1f);
}

void TacticalPane::MouseLeftUpRay(Ray ray)
{
	Tile *tile= GetTile(ray);
	if(tile!= nullptr && tile->GetActor()!= nullptr)
		selected_actor= tile->GetActor();
}

void TacticalPane::MouseMotionRay(Ray ray, Ray last_ray)
{
	hovered_tile= GetTile(ray);
}

void TacticalPane::MouseScroll(int scroll_amount)
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

void TacticalPane::MouseIn()
{
	is_hovered= true;
}

void TacticalPane::MouseOut()
{
	is_hovered= false;
}

void TacticalPane::Draw()
{
	CameraPane::Draw();

	if(hovered_tile!= nullptr)
	{
		Vec3f tile_position= hovered_tile->GetPosition();

		ShaderProgram *shader_program= ShaderProgram::Retrieve("lines.program");
		shader_program->Use();
		shader_program->SetUniformMatrix4x4f("model_transform", Transform().Translate(Vec3f(tile_position.x, tile_position.y, tile_position.z)).GetMatrix());
		shader_program->SetUniformMatrix4x4f("camera_transform", GetCamera()->GetProjectedTransform(Math::GetAspectRatio(GetPixelSize())).GetMatrix());
		if(is_hovered)
			shader_program->SetUniformVector3f("line_color", Vec3f(0, 1, 0));
		else
			shader_program->SetUniformVector3f("line_color", Vec3f(1, 0, 0));

		AttachDepth();
		Viewport::Push(GetPixelOffset(), GetPixelSize());
		tile_frame_vao.Bind();
		glDrawArrays(GL_LINES, 0, 24);
		Viewport::Pop();


	}

	button.Draw();
}
