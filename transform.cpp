#include "Transform.h"

void Transform::Push(const Mat4x4f &matrix_, const Mat4x4f &inverse_matrix_, bool push_bottom)
{
	if(!push_bottom)
	{
		matrix= matrix_* matrix;
		inverse_matrix= inverse_matrix* inverse_matrix_;
	}
	else
	{
		matrix= matrix* matrix_;
		inverse_matrix= inverse_matrix_* inverse_matrix;
	}
}

Transform::Transform()
{
	matrix= inverse_matrix= Mat4x4f::MakeIdentity();
}

Transform Transform::Copy()
{
	Transform transform;
	transform= *this;

	return transform;
}

Mat4x4f Transform::GetMatrix()
{
	return matrix;
}

Mat4x4f Transform::GetInverseMatrix()
{
	return inverse_matrix;
}

Transform & Transform::Invert()
{
	Mat4x4f temp= matrix;
	matrix= inverse_matrix;
	inverse_matrix= temp;

	return *this;
}

Transform Transform::Inverted()
{
	return Copy().Invert();
}


Transform & Transform::Merge(Transform other, bool reverse_order)
{
	if(!reverse_order)
	{
		matrix= other.matrix* matrix;
		inverse_matrix= inverse_matrix* other.inverse_matrix;
	}
	else
	{
		matrix= matrix* other.matrix;
		inverse_matrix= other.inverse_matrix* inverse_matrix;
	}

	is_invertible= is_invertible&& other.is_invertible;//see whether you can use &=

	return *this;
}

Transform Transform::Merged(Transform other, bool reverse_order)
{
	return Copy().Merge(other, reverse_order);
}

Transform & Transform::RotateAboutX(float theta, bool reverse_order)
{
	float c= cos(theta);
	float s= sin(theta);
	float n= -s;

	float cells[16]= { 1, 0, 0, 0, 
						0, c, n, 0, 
						0, s, c, 0,
						0, 0, 0, 1 };

	Mat4x4f matrix= cells;
	Push(matrix, matrix.Transposed(), reverse_order);

	return *this;
}

Transform Transform::RotatedAboutX(float theta, bool reverse_order)
{
	return Copy().RotateAboutX(theta, reverse_order);
}

Transform & Transform::RotateAboutY(float theta, bool reverse_order)
{
	float c= cos(theta);
	float s= sin(theta);
	float n= -s;

	float cells[16]= { c, 0, s, 0,
					   0, 1, 0, 0, 
					   n, 0, c, 0,
					   0, 0, 0, 1 };

	Mat4x4f matrix= cells;
	Push(matrix, matrix.Transposed(), reverse_order);

	return *this;
}

Transform Transform::RotatedAboutY(float theta, bool reverse_order)
{
	return Copy().RotateAboutY(theta, reverse_order);
}

Transform & Transform::RotateAboutZ(float theta, bool reverse_order)
{
	float c= cos(theta);
	float s= sin(theta);
	float n= -s;

	float cells[16]= { c, n, 0, 0,
					   s, c, 0, 0, 
					   0, 0, 1, 0,
					   0, 0, 0, 1 };

	Mat4x4f matrix= cells;
	Push(matrix, matrix.Transposed(), reverse_order);

	return *this;
}

Transform Transform::RotatedAboutZ(float theta, bool reverse_order)
{
	return Copy().RotateAboutZ(theta, reverse_order);
}

//Adapted from CyMatrix4.h http://www.cemyuksel.com/cyCodeBase/code.html#cyMatrix
Transform & Transform::RotateAboutAxis(float theta, Vec3f axis, bool reverse_order)
{
	float c = (float) cos(theta);
	if ( c == 1 )
		return *this;

	float s = (float) sin(theta);

	float t = 1 - c;
	float tx = t * axis.x;
	float ty = t * axis.y;
	float tz = t * axis.z;
	float txy = tx * axis.y;
	float txz = tx * axis.z;
	float tyz = ty * axis.z;
	float sx = s * axis.x;
	float sy = s * axis.y;
	float sz = s * axis.z;

	float cells[16]= { tx * axis.x + c,		txy - sz,			txz + sy,			0, 
					   txy + sz,			ty * axis.y + c,	tyz - sx,			0, 
					   txz - sy,			tyz + sx,			tz * axis.z + c,	0,
					   0,					0,					0,					1 };

	Mat4x4f matrix= cells;
	Push(matrix, matrix.Transposed(), reverse_order);

	return *this;
}

Transform Transform::RotatedAboutAxis(float theta, Vec3f axis, bool reverse_order)
{
	return Copy().RotateAboutAxis(theta, axis, reverse_order);
}

Transform & Transform::Scale(float scale, bool reverse_order)
{
	float s= scale;

	float cells[16]= { s, 0, 0, 0, 
					   0, s, 0, 0, 
					   0, 0, s, 0,
					   0, 0, 0, 1 };

	float inverse_cells[16]= { 1/ s, 0,    0,    0, 
							   0,    1/ s, 0,    0, 
							   0,    0,    1/ s, 0,
							   0,    0,    0,    1 };

	Push(cells, inverse_cells, reverse_order);

	return *this;
}

Transform Transform::Scaled(float scale, bool reverse_order)
{
	return Copy().Scale(scale, reverse_order);
}

Transform & Transform::Translate(Vec3f displacement, bool reverse_order)
{
	float x= displacement.x;
	float y= displacement.y;
	float z= displacement.z;

	float cells[16]= { 1, 0, 0, x, 
					   0, 1, 0, y, 
					   0, 0, 1, z,
					   0, 0, 0, 1 };

	float inverse_cells[16]= { 1, 0, 0, -x, 
							   0, 1, 0, -y, 
							   0, 0, 1, -z,
							   0, 0, 0,  1 };

	Push(cells, inverse_cells, reverse_order);

	return *this;
}

Transform Transform::Translated(Vec3f displacement, bool reverse_order)
{
	return Copy().Translate(displacement, reverse_order);
}

//Does not account for w coordinate
Vec3f Transform::Apply(Vec3f vector_, bool is_direction)
{
	return (matrix* vector_.Push(is_direction ? 0.0f : 1.0f)).Pop();
}

//fov is measured vertically
Transform Transform::MakeProjectionTransform(float fov, float aspect_ratio)
{
	float f= 1/ tan(fov/ 2);

	float near= 0.1f;
	float far= 1000.0;

	float a= (near+ far)/ (near- far);
	float b= (2* near* far)/ (near- far);

	float cells[16] = { f/ aspect_ratio,	0,  0,  0, 
						0,					f,  0,  0, 
						0,					0,  a,  b,
						0,					0,	-1,  0 };

	Transform transform;
	transform.matrix= cells;
	transform.is_invertible= false;

	return transform;
}