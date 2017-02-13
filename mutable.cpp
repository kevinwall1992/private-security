#include "Mutable.h"

bool Mutable::WasModified()
{
	return was_modified;
}

void Mutable::Touch()
{
	was_modified= true;
}

void Mutable::Scrub()
{
	was_modified= false;
}


void Sensor::Touch()
{
	if(parent!= nullptr)
	{
		if(callback!= nullptr)
			callback();
		else
			parent->Touch();
	}
}

Sensor::Sensor(Mutable *parent, Callback callback_)
{
	this->parent= parent;
	this->callback= callback_;
}


bool Checkable::WasModified()
{
	if(Mutable::WasModified())
	{
		Scrub();
		return true;
	}
	else
		return false;
}


void Updatable::Update()
{

}

void Updatable::CatchUp()
{
	if(WasModified())
		Update();
}
