#ifndef PS_VECTOR
#define PS_VECTOR

#include "Property.h"

#include <functional>


template<int w, int h, class T>
struct Matrix;

template<int d, class T>
struct Vector
{
	T c[d];

	template<class U>
	Vector<d, T> & Mutate(const Vector<d, U> &other, std::function<T(T, U)> function)
	{
		for(int i= 0; i< d; i++)
			c[i]= (T)function(c[i], other.c[i]);

		return *this;
	}

	template<class U>
	Vector<d, T> Mutated(const Vector<d, U> &other, std::function<T(T, U)> function)
	{
		Vector<d, T> result;

		for(int i= 0; i< d; i++)
			result.c[i]= (T)function(c[i], other.c[i]);

		return result;
	}

	template<class U>
	Vector<d, U> & Mutate(std::function<U(U)> function)
	{
		for(int i= 0; i< d; i++)
			this->c[i]= function((U)c[i]);

		return *this;
	}

	template<class U>
	Vector<d, U> Mutated(std::function<U(U)> function)
	{
		Vector<d, U> result;

		for(int i= 0; i< d; i++)
			result.c[i]= function((U)c[i]);

		return result;
	}

	T Sum()
	{
		T sum= 0;

		for(int i= 0; i< d; i++)
			sum+= c[i];

		return sum;
	}

	template<class U>
	U Sum(std::function<U(U)> function)
	{
		return Mutated<U>(function).Sum();
	}

	Vector()
	{
		for(int i= 0; i< d; i++)
			c[i]= 0;
	}

	Vector(T *components)
	{
		memcpy(c, components, sizeof(T)* d);
	}

	Vector(const Vector<d, T> &other)
	{
		memcpy(this->c, other.c, sizeof(T)* d);
	}

	Vector(const Matrix<1, d, T> &matrix)
	{
		memcpy(this->c, matrix.c, sizeof(T)* d);
	}

	T & operator[](const int index)
	{
		return c[index];
	}

	T operator[](const int index) const
	{
		return c[index];
	}

	Vector<d, T> & operator=(const Vector<d, T> &other)
	{
		memcpy(this->c, other.c, sizeof(T)* d);
		return *this;
	}

	template<class U>
	operator Vector<d, U>()
	{
		return Mutated<U>([](U b){ return b; });
	}

	bool operator==(const Vector<d, T> &other)
	{
		for(int i= 0; i< d; i++)
			if(this->c[i]!= other.c[i])
				return false;
		
		return true;
	}

	bool operator!=(const Vector<d, T> &other)
	{
		return !(*this== other);
	}

	Vector<d+ 1, T> Push(T element= T())
	{
		Vector<d+ 1, T> vector;
		memcpy(vector.c, c, sizeof(T)* d);
		vector.c[d]= element;

		return vector;
	}

	Vector<d- 1, T> Pop()
	{
		Vector<d- 1, T> vector;
		memcpy(vector.c, c, sizeof(T)* (d- 1));

		return vector;
	}


	Vector<d, T> operator+(const Vector<d, T> &b)
	{
		return Mutated<T>(b, [](T a, T b){ return a+ b; });
	}

	Vector<d, T> & operator+=(const Vector<d, T> &b)
	{
		return (*this= (*this+ b));
	}

	Vector<d, T> operator-(const Vector<d, T> &b)
	{
		return Mutated<T>(b, [](T a, T b){ return a- b; });
	}

	Vector<d, T> & operator-=(const Vector<d, T> &b)
	{	
		return (*this= (*this- b));
	}

	Vector<d, T> operator-()
	{
		return Mutated<T>([](T a){ return -a; });
	}

	Vector<d, T> operator*(const T &scalar)
	{
		return Mutated<T>([&scalar](T a){ return a* scalar; });
	}

	Vector<d, T> & operator*=(const T &scalar)
	{
		return (*this= (*this* scalar));
	}

	Vector<d, T> operator*(const Vector<d, T> &b)
	{
		return Mutated<T>(b, [](T a, T b){ return a* b; });
	}

	Vector<d, T> & operator*=(const Vector<d, T> &b)
	{
		return (*this= (*this* b));
	}

	Vector<d, T> operator/(const T &scalar)
	{
		return Mutated<T>([&scalar](T a){ return a/ scalar; });
	}

	Vector<d, T> & operator/=(const T &scalar)
	{
		return (*this= (*this/ scalar));
	}

	Vector<d, T> operator/(const Vector<d, T> &b)
	{
		return Mutated<T>(b, [](T a, T b){ return a/ b; });
	}

	Vector<d, T> & operator/=(const Vector<d, T> &b)
	{
		return (*this= (*this/ b));
	}

	T Dot(const Vector<d, T> &other)
	{
		return Mutated<T>(other, [](T a, T b){ return a* b; }).Sum();
	}

	float Magnitude()
	{
		return (float)sqrt(Sum<float>([](float a){ return (float)pow((float)a, 2); }));
	}

	float Distance(const Vector<d, T> &other)
	{
		return (*this- other).Magnitude();
	}

	Vector<d, T> Normalized()
	{
		return *this/ Magnitude();
	}

	Vector<d, T> & Normalize()
	{
		return (*this/= Magnitude());
	}
};


#define PROPERTY_CONSTRUCTORS x(c+ 0), y(c+ 1), z(c+ 2)

template<class T>
struct Vector4: public Vector<4, T>
{
	Property<T> x, y, z, w;

	Vector4(T x, T y, T z, T w)
		: PROPERTY_CONSTRUCTORS
	{
		this->x= x;
		this->y= y;
		this->z= z;
		this->w= w;
	}

	Vector4(T *components)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, components, sizeof(T)* 4);
	}

	Vector4(const Vector4<T>& other)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, other.c, sizeof(T)* 4);
	}

	Vector4(Vector4<float> vector, bool translatable= true)
		: PROPERTY_CONSTRUCTORS
	{
		x= vector.x;
		y= vector.y;
		z= vector.z;

		if(translatable)
			w= 1;
		else
			w= 0;
	}

	template<class U>
	Vector4<T>(const Vector<4, U> &other)
		: PROPERTY_CONSTRUCTORS
	{
		x= (T)other.c[0];
		y= (T)other.c[1];
		z= (T)other.c[2];
		w= (T)other.c[3];
	}

	Vector4()
		: PROPERTY_CONSTRUCTORS
	{

	}

	template<class U>
	Vector4<T> & operator=(const Vector<4, U> &other)
	{
		x= (T)other.c[0];
		y= (T)other.c[1];
		z= (T)other.c[2];
		w= (T)other.c[3];

		return *this;
	}
};

#undef PROPERTY_CONSTRUCTORS


#define PROPERTY_CONSTRUCTORS x(c+ 0), y(c+ 1), z(c+ 2)

template<class T>
struct Vector3: public Vector<3, T>
{
	Property<T> x, y, z;

	Vector3(T x, T y, T z)
		: PROPERTY_CONSTRUCTORS
	{
		this->x= x;
		this->y= y;
		this->z= z;
	}

	Vector3(T *components)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, components, sizeof(T)* 3);
	}

	Vector3(const Vector3<T>& other)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, other.c, sizeof(T)* 3);
	}

	template<class U>
	Vector3(const Vector<3, U> &other)
		: PROPERTY_CONSTRUCTORS
	{
		x= (T)other.c[0];
		y= (T)other.c[1];
		z= (T)other.c[2];
	}

	Vector3()
		: PROPERTY_CONSTRUCTORS
	{
		
	}

	Vector3<T> & operator=(const Vector<3, T> &other)
	{
		memcpy(this->c, other.c, sizeof(T)* 3);

		return *this;
	}

	Vector3<T> & operator=(const Vector3<T> &other)
	{
		memcpy(this->c, other.c, sizeof(T)* 3);

		return *this;
	}

	Vector3<T> Cross(Vector3<T> &other)
	{
		return Vector3<T>(this->y* other.z- this->z* other.y, this->z* other.x- this->x* other.z, this->x* other.y- this->y* other.x);
	}
};

#undef PROPERTY_CONSTRUCTORS


#define PROPERTY_CONSTRUCTORS x(c+ 0), y(c+ 1)

template<class T>
struct Vector2: public Vector<2, T>
{
	Property<T> x, y;

	Vector2(T x, T y)
		: PROPERTY_CONSTRUCTORS
	{
		this->x= x;
		this->y= y;
	}

	Vector2(T *components)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, components, sizeof(T)* 2);
	}

	Vector2(const Vector2<T>& other)
		: PROPERTY_CONSTRUCTORS
	{
		memcpy(c, other.c, sizeof(T)* 2);
	}

	template<class U>
	Vector2<T>(const Vector<2, U> &other)
		: PROPERTY_CONSTRUCTORS
	{
		x= (T)other.c[0];
		y= (T)other.c[1];
	}

	Vector2()
		: PROPERTY_CONSTRUCTORS
	{

	}

	template<class U>
	Vector2<T> & operator=(const Vector<2, U> &other)
	{
		x= (T)other.c[0];
		y= (T)other.c[1];

		return *this;
	}

	Vector2<T> Cross(Vector2<T> &other)
	{
		
	}
};

#undef PROPERTY_CONSTRUCTORS


typedef Vector4<float> Vec4f;
typedef Vector4<int> Vec4i;

typedef Vector3<float> Vec3f;
typedef Vector3<int> Vec3i;

typedef Vector2<float> Vec2f;
typedef Vector2<int> Vec2i;

#endif
