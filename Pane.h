#ifndef PS_PANE
#define PS_PANE

#include "Interface.h"
#include "Transform.h"
#include "PropertySpecializations.h"
#include "Texture.h"
#include "Camera.h"
#include "Shader.h"
#include "Scene.h"
#include "Gizmos.h"
#include "Frame.h"
#include "Viewport.h"


class Pane : public Interface, public TreeFrame
{
protected:

	//Pixel measure is always in screen space
	Vec2i LocalPositionToPixelPosition(Vec2f position);
	Vec2i LocalSizeToPixelSize(Vec2f size);
	Vec2f PixelPositionToLocalPosition(Vec2i position);
	Vec2f PixelSizeToLocalSize(Vec2i size);

	//"Screen" size/position means normalized to screen dimensions
	Vec2f ScreenPositionToLocalPosition(Vec2f position, bool snap_to_pixel= false);
	Vec2f ScreenSizeToLocalSize(Vec2f size, bool snap_to_pixel= false);
	Vec2f LocalPositionToScreenPosition(Vec2f position, bool snap_to_pixel= false);
	Vec2f LocalSizeToScreenSize(Vec2f size, bool snap_to_pixel= false);
	
	Vec2i GetPixelOffset();
	Vec2i GetPixelSize();

	float GetAspectRatio();

	virtual bool IsInside(Vec2f point);

public:
	Pane(Vec2f offset, Vec2f size);
	Pane();
};


class QuadPane : public Pane
{
protected:
	Transform GetQuadTransform(bool snap_to_pixel);

	virtual ShaderProgram * GetShaderProgram()= 0;
	virtual void UploadShaderUniforms()= 0;
	
public:
	virtual void Draw();
};


class TexturePane : public QuadPane
{
	ShaderProgram * GetShaderProgram();
	void UploadShaderUniforms();

protected:
	virtual Texture GetTexture()= 0;

public:
	virtual void Draw();
};


class CameraPane : public TexturePane
{
	Camera *camera;
	Scene *scene;

	DepthTexture depth_texture;

protected:
	Camera * GetCamera();

	Texture GetTexture();

	Ray GetRay(Vec2f mouse_position);
	
	void AttachDepth();

	virtual void HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event);
	virtual void HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event);

	virtual void MouseLeftUpRay(Ray ray);
	virtual void MouseMiddleUpRay(Ray ray);
	virtual void MouseRightUpRay(Ray ray);

	virtual void MouseLeftDownRay(Ray ray);
	virtual void MouseMiddleDownRay(Ray ray);
	virtual void MouseRightDownRay(Ray ray);

	virtual void MouseLeftDoubleClickRay(Ray ray);
	virtual void MouseMiddleDoubleClickRay(Ray ray);
	virtual void MouseRightDoubleClickRay(Ray ray);

	virtual void MouseMotionRay(Ray ray, Ray last_ray);

public:
	CameraPane(Camera *camera);

	void SetScene(Scene *scene);
};

#endif