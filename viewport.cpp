#include "Viewport.h"
#include "GraphicsSystem.h"


std::stack<Viewport> Viewport::viewport_stack;
Viewport Viewport::default_viewport;

void Viewport::InitializeDefaultViewport()
{
	if(default_viewport.size.x!= -1)
		return;

	default_viewport= Viewport(Vec2i(0, 0), System::graphics.GetScreenSize());
	default_viewport.Apply();
}

Viewport::Viewport(Vec2i offset_, Vec2i size_)
{
	offset= offset_;
	size= size_;
}

Viewport::Viewport()
{
	size= Vec2i(-1, -1);
}

void Viewport::Apply()
{
	glViewport(offset.x, offset.y, size.x, size.y);
}

Vec2i Viewport::GetOffset()
{
	return offset;
}

Vec2i Viewport::GetSize()
{
	return size;
}

void Viewport::Set(Vec2i offset, Vec2i size)
{
	if(viewport_stack.size()> 0)
	{
		Viewport top= Peek();

		top.offset= offset;
		top.size= size;
		top.Apply();
	}
	else
		Push(offset, size);
}

void Viewport::Set(int x_offset, int y_offset, int width, int height)
{
	Set(Vec2i(x_offset, y_offset), Vec2i(width, height));
}

void Viewport::Push(Vec2i offset, Vec2i size)
{
	InitializeDefaultViewport();

	viewport_stack.push(Viewport(offset, size));

	Peek().Apply();
}

void Viewport::Push(int x_offset, int y_offset, int width, int height)
{
	Push(Vec2i(x_offset, y_offset), Vec2i(width, height));
}

void Viewport::Push()
{
	InitializeDefaultViewport();

	viewport_stack.push(Peek());
}

Viewport Viewport::Peek()
{
	InitializeDefaultViewport();

	if(viewport_stack.size()> 0)
		return viewport_stack.top();
	else
	{
		return default_viewport;
	}
}

Viewport Viewport::Pop()
{
	InitializeDefaultViewport();

	Viewport viewport= Peek();
	if(viewport_stack.size()> 0)
	{
		viewport_stack.pop();

		Peek().Apply();
	}

	return viewport;
}
