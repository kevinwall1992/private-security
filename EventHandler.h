#ifndef PS_EVENT_HANDLER
#define PS_EVENT_HANDLER

#include "Event.h"


class EventHandler
{
public:
	virtual void HandleEvent(Event *event)= 0;
};

#endif