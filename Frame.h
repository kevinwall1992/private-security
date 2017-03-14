#ifndef PS_RECTANGULAR
#define PS_RECTANGULAR

#include "Vector.h"
#include "Treelike.h"
#include "PropertySpecializations.h"


class Frame
{
	Vec2f offset, size;

public:
	Vec2fProperty Offset, Size;

	Frame(Vec2f offset, Vec2f size);
	Frame();
};

struct TreeFrame : public Frame, public Treelike<TreeFrame>
{

protected:
	bool IsRoot();

public:
	TreeFrame(Vec2f offset, Vec2f size);
	TreeFrame();

	//void SetParent(TreeFrame *parent);

	Vec2f GetLocalOffset();
	Vec2f GetLocalSize();
	Vec2f GetGlobalOffset();
	Vec2f GetGlobalSize();

	Vec2f ChildPositionToParentPosition(Vec2f position);
	Vec2f ChildSizeToParentSize(Vec2f size);
	Vec2f ParentPositionToChildPosition(Vec2f position);
	Vec2f ParentSizeToChildSize(Vec2f size);

	Vec2f LocalPositionToGlobalPosition(Vec2f position);
	Vec2f LocalSizeToGlobalSize(Vec2f size);
	Vec2f GlobalPositionToLocalPosition(Vec2f position);
	Vec2f GlobalSizeToLocalSize(Vec2f size);
};

#endif
