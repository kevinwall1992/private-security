#include "GarbageCan.h"


vector<GarbageCanBase *> GarbageCanBase::garbage_cans;

GarbageCanBase::GarbageCanBase()
{
	garbage_cans.push_back(this);
}

void GarbageCanBase::CollectAll()
{
	for(GarbageCanBase *garbage_can : garbage_cans)
		garbage_can->Collect();
}

void Landfill::Dump()
{
	GarbageCanBase::CollectAll();
}

