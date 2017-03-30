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
#include "Drawables.h"


class Pane : public Interface, public TreeFrame
{
protected:

	virtual void AddComponent(Pane *pane);
	virtual void AddComponent(Interface *interface_);
	virtual void AddComponent(Drawable *drawable);
	virtual void AddComponent(Gizmo *gizmo);

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
	bool use_depth= false;

protected:
	Transform GetQuadTransform(bool snap_to_pixel);

	virtual ShaderProgram * GetShaderProgram()= 0;
	virtual void UploadShaderUniforms()= 0;

	void UseDepth();
	void DontUseDepth();
	
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


class TextPane : public Pane
{
public:
	class Group
	{
		vector<TextPane *> text_panes;

	public:
		int GetMaximumFontSize();

		void AddTextPane(TextPane *text_pane);
	};

	enum Align { TopLeft, TopCenter, TopRight, CenterLeft, Center, CenterRight, BottomLeft, BottomCenter, BottomRight };

private:

	Group *group= nullptr;
	vector<TextDrawable *> text_drawables;
	Align align;

	string text;
	Color color;

	int font_size= -1;
	bool font_size_locked= false;

	void GenerateTextDrawables();

protected:
	int GetLocalMaximumFontSize();
	int GetMaximumFontSize();

	void SetGroup(Group *group);

public:
	TextPane(string text, Color color, Align align= Align::Center);
	TextPane();

	void SetFontSize(int font_size);

	void LockFontSize();
	void UnlockFontSize();
	bool IsFontSizeLocked();

	void SetText(string text, Color color, Align align= Align::Center);

	virtual void Draw();
};

#endif