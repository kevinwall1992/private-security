#include "Chronon.h"

Chronons SecondsToChronons(float seconds)
{
	return (int)(seconds* 1000);
}

float ChrononsToSeconds(Chronons chronons)
{
	return chronons/ 1000.0f;
}
