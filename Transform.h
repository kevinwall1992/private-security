#ifndef PS_TRANSFORM
#define PS_TRANSFORM

#include "Matrix.h"


class Transform
{
	Mat4x4f matrix;
	Mat4x4f inverse_matrix;
	bool is_invertible;

	void Push(const Mat4x4f &matrix, const Mat4x4f &inverse_matrix, bool push_bottom= false);

public:
	Transform();

	Transform Copy();

	Mat4x4f GetMatrix();
	Mat4x4f GetInverseMatrix();

	Transform & Invert();
	Transform Inverted();

	virtual Transform & Merge(Transform other, bool reverse_order= false);
	virtual Transform Merged(Transform other, bool reverse_order= false);

	virtual Transform & RotateAboutX(float theta, bool reverse_order= false);
	virtual Transform RotatedAboutX(float theta, bool reverse_order= false);
	virtual Transform & RotateAboutY(float theta, bool reverse_order= false);
	virtual Transform RotatedAboutY(float theta, bool reverse_order= false);
	virtual Transform & RotateAboutZ(float theta, bool reverse_order= false);
	virtual Transform RotatedAboutZ(float theta, bool reverse_order= false);
	virtual Transform & RotateAboutAxis(float theta, Vec3f axis, bool reverse_order= false);
	virtual Transform RotatedAboutAxis(float theta, Vec3f axis, bool reverse_order= false);

	virtual Transform & Scale(float scale, bool reverse_order= false);
	virtual Transform Scaled(float scale, bool reverse_order= false);
	virtual Transform & Scale(Vec3f scale, bool reverse_order= false);
	virtual Transform Scaled(Vec3f scale, bool reverse_order= false);

	virtual Transform & Translate(Vec3f displacement, bool reverse_order= false);
	virtual Transform Translated(Vec3f displacement, bool reverse_order= false);

	Vec3f Apply(Vec3f vector_, bool is_direction= false);

	static Transform MakeProjectionTransform(float fov, float aspect_ratio);
};

#endif
