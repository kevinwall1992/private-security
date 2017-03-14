#include "Ninepatch.h"
#include "ImageFile.h"


void Ninepatch::SetRoundness(Roundness roundness)
{
	switch(roundness)
	{
	case Roundness::Sharp: corner_size= 0.0045f; break;
	case Roundness::Soft: corner_size= 0.01f; break;
	case Roundness::Normal:
	default: corner_size= 0.0065f; break;
	}

	corner_size*= (System::graphics.GetScreenSize().x/ 1024.0f);
}

Ninepatch::Ninepatch(Texture texture_, Vec2f offset, Vec2f size, Roundness roundness)
	: Pane(offset, size)
{
	texture= texture_;

	SetRoundness(roundness);
}

Ninepatch::Ninepatch(string image_filename, Roundness roundness)
{
	texture= Texture(ImageFile::Retrieve(image_filename)->MakeImage());

	SetRoundness(roundness);
}

void Ninepatch::Draw()
{
	Vec2f global_offset= LocalPositionToGlobalPosition(GetLocalOffset());
	Vec2f global_size= LocalSizeToGlobalSize(GetLocalSize());

	Vec2f corner_scale= Vec2f(corner_size, corner_size)/ global_size;
	Vec2f edge_scale= (Vec2f(1, 1)- (corner_scale* 2));

	ShaderProgram *shader_program= ShaderProgram::Retrieve("quad.program");
	shader_program->Use();
	texture.BindToIndex(0);

	glDisable(GL_DEPTH_TEST);

	for(int i= 0; i< 9; i++)
	{
		int x= i% 3;
		int y= i/ 3;

		Vec2f patch_offset;

		if(x> 0)
			patch_offset.x+= corner_scale.x;
		if(x> 1)
			patch_offset.x+= edge_scale.x;
		if(y> 0)
			patch_offset.y+= corner_scale.y;
		if(y> 1)
			patch_offset.y+= edge_scale.y;


		Vec2f patch_size;

		//Middle
		if(x== 1 && y== 1)
		{
			patch_size= edge_scale;
		}

		//Edge
		else if(x== 1 || y== 1)
		{
			if(x== 1)
			{
				patch_size.x= edge_scale.x;
				patch_size.y= corner_scale.y;
			}
			else
			{
				patch_size.x= corner_scale.x;
				patch_size.y= edge_scale.y;
			}
		}

		//Corner
		else
		{
			patch_size= corner_scale;
		}

		shader_program->SetUniformMatrix4x4f("transform", Transform().Translate(Vec3f(1, 1, 0)).Scale(patch_size.Push()/ 2).Translate(patch_offset.Push()).Scale(global_size.Push()).Translate(global_offset.Push()).Scale(2).Translate(Vec3f(-1, -1, 0)));
		shader_program->SetUniformMatrix4x4f("texture_transform", Transform().Scale(1/ 3.0f).Translate(Vec3f(x/ 3.0f, y/ 3.0f, 0.0f)));
		RasterizeFullScreenQuad();
	}

	glEnable(GL_DEPTH_TEST);
}