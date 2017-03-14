#ifndef PS_VIEWPORT
#define PS_VIEWPORT

#include "GraphicsLibraries.h"
#include "Common.h"
#include "Vector.h"

#include <stack>


class Viewport
{
	static std::stack<Viewport> viewport_stack;
	static Viewport default_viewport;

	static void InitializeDefaultViewport();

	Vec2i offset;
	Vec2i size;

	Viewport(Vec2i offset, Vec2i size);

	void Apply();

public:
	Viewport();

	Vec2i GetOffset();
	Vec2i GetSize();

	static void Set(Vec2i offset, Vec2i size);
	static void Set(int x_offset, int y_offset, int width, int height);

	static void Push(Vec2i offset, Vec2i size);
	static void Push(int x_offset, int y_offset, int width, int height);
	static void Push();

	static Viewport Peek();

	static Viewport Pop();
};

#endif