#ifndef PS_CHRONON
#define PS_CHRONON

typedef int Chronons;

Chronons SecondsToChronons(float seconds);
float ChrononsToSeconds(Chronons chronons);

class Chronal
{
public:
	//Consider changing to "step" instead of "chronons"
	//using "chronon" in variable names is clunky and so I've 
	//started just calling them what they are, "chronon" is implied
	//by their type
	virtual void Step(Chronons chronons)= 0;
};

#endif
