#include "Drawable.h"


void Drawable::Show()
{
	is_hidden= false;
}

void Drawable::Hide()
{
	is_hidden= true;
}

bool Drawable::IsHidden()
{
	return is_hidden;
}
