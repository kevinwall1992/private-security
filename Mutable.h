#ifndef PS_UPDATABLE
#define PS_UPDATABLE

class Mutable
{
	bool was_modified= true;

public:
	virtual bool WasModified();

	virtual void Touch();
	virtual void Scrub();
};


class Sensor : public Mutable
{
public:
	typedef void (*Callback)();

private:
	Mutable *parent;
	Callback callback;

	using Mutable::Scrub;

protected:
	void Touch();

	using Mutable::WasModified;

public:
	Sensor(Mutable *parent, Callback callback= nullptr);
};


class Checkable : public Mutable
{
	using Mutable::Scrub;
	
protected:
	using Mutable::Touch;

public:
	bool WasModified();
};


class Updatable : public Checkable
{
	using Checkable::WasModified;

protected:
	virtual void Update();
	void CatchUp();

public:

};

#endif