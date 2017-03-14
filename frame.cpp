#include "Frame.h"


Frame::Frame(Vec2f offset_, Vec2f size_)
	: Offset(&offset), Size(&size)
{
	Offset= offset_;
	Size= size_;
}

Frame::Frame()
	: Offset(&offset), Size(&size)
{
}

bool TreeFrame::IsRoot()
{
	return GetParent()== nullptr;
}

TreeFrame::TreeFrame(Vec2f offset, Vec2f size)
	: Frame(offset, size)
{

}

TreeFrame::TreeFrame()
{

}

Vec2f TreeFrame::GetLocalOffset()
{
	return Vec2f(0.0f, 0.0f);
}

Vec2f TreeFrame::GetLocalSize()
{
	return Vec2f(1.0f, 1.0f);
}

Vec2f TreeFrame::GetGlobalOffset()
{
	return LocalPositionToGlobalPosition(GetLocalOffset());
}

Vec2f TreeFrame::GetGlobalSize()
{
	return LocalSizeToGlobalSize(GetLocalSize());
}

Vec2f TreeFrame::ChildPositionToParentPosition(Vec2f position)
{
	return position* Size+ Offset;
}

Vec2f TreeFrame::ChildSizeToParentSize(Vec2f size)
{
	return size* Size;
}

Vec2f TreeFrame::ParentPositionToChildPosition(Vec2f position)
{
	return (position- Offset)/ Size;
}

Vec2f TreeFrame::ParentSizeToChildSize(Vec2f size)
{
	return size/ Size;
}

Vec2f TreeFrame::LocalPositionToGlobalPosition(Vec2f position)
{
	Vec2f parent_position= ChildPositionToParentPosition(position);

	if(IsRoot())
		return parent_position;
	else
		return GetParent()->LocalPositionToGlobalPosition(parent_position);
}

Vec2f TreeFrame::LocalSizeToGlobalSize(Vec2f size)
{
	Vec2f parent_size= ChildSizeToParentSize(size);

	if(IsRoot())
		return parent_size;
	else
		return GetParent()->LocalSizeToGlobalSize(parent_size);
}

Vec2f TreeFrame::GlobalPositionToLocalPosition(Vec2f position)
{
	Vec2f parent_position;

	if(IsRoot())
		parent_position= position;
	else
		parent_position= GetParent()->GlobalPositionToLocalPosition(position);

	return ParentPositionToChildPosition(parent_position);
}

Vec2f TreeFrame::GlobalSizeToLocalSize(Vec2f size)
{
	Vec2f parent_size;

	if(IsRoot())
		parent_size= size;
	else
		parent_size= GetParent()->GlobalSizeToLocalSize(size);

	return ParentSizeToChildSize(parent_size);
}