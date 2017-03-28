#ifndef PS_DRAWABLES
#define PS_DRAWABLES

#include "Drawable.h"
#include "Transform.h"
#include "Texture.h"
#include "Common.h"
#include "Color.h"
#include "Font.h"


class TextDrawable : public Drawable
{
	Font *font;
	Transform transform;

	string text;
	int font_size;
	Color color;

	Texture text_texture;

	void GenerateTextTexture();

public:
	TextDrawable(string text, int font_size, Color color);

	Vec2i GetTextureSize();
	float GetAspectRatio();

	void SetTransform(Transform transform);

	virtual void Draw();
};


#endif