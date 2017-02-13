#ifndef PS_MATRIX
#define PS_MATRIX

#include "Vector.h"


template<int w, int h, class T>
struct Matrix
{
	template<int w>
	struct Column
	{
		T *c;

		Column(T *c)
		{
			this->c= c;
		}

		T & operator[](const int index)
		{
			return c[index* w];
		}
	};

	T c[w* h];

	Matrix()
	{
		for(int j= 0; j< h; j++)
			for(int i= 0; i< w; i++)
				c[i+ j* w]= 0;
	}

	Matrix(T *cells)
	{
		memcpy(this->c, cells, sizeof(T)* w* h);
	}

	Matrix(const Matrix<w, h, T> &other)
	{
		memcpy(this->c, other.c, sizeof(T)* w* h);
	}

	Matrix(const Vector<h, T> &vector)
	{
		memcpy(this->c, vector.c, sizeof(T)* h);
	}

	Column<w> operator[](const int index)
	{
		return Column<w>(c+ index);
	}

	template<int p>
	Matrix<p, h, T> operator*(const Matrix<p, w, T> &b) const
	{
		Matrix<p, h, T> result;

		for(int j= 0; j< h; j++)
			for(int i= 0; i< p; i++)
				for(int k= 0; k< w; k++)
					result.c[i+ j* p]+= this->c[k+ j* w]* b.c[i+ k* p];

		return result;
	}

	Vector<w, T> operator*(const Vector<w, T> &v) const
	{
		return Vector<w, T>(*this* Matrix<1, w, T>(v));
	}

	Matrix<w, w, T> Transposed()
	{
		Matrix<w, w, T> transposed;

		for(int j= 0; j< w; j++)
			for(int i= 0; i< w; i++)
				transposed.c[i+ j* w]= this->c[j+ i* w];

		return transposed;
	}

	Matrix<w, w, T> & Transpose()
	{
		*this= Transposed();

		return *this;
	}

	static Matrix<w, w, T> MakeIdentity()
	{
		Matrix<w, w, T> matrix;

		for(int i= 0; i< w; i++)
			matrix[i][i]= 1;

		return matrix;
	}
};

typedef Matrix<4, 4, float> Mat4x4f;

#endif