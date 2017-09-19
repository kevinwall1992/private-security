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
	virtual void AddComponent(Pane *pane, bool send_to_back= false);
	virtual void AddComponent(Interface *interface_, bool send_to_back= false);
	virtual void AddComponent(Drawable *drawable, bool send_to_back= false);
	virtual void AddComponent(Gizmo *gizmo, bool send_to_back= false);

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

	Transform GetTransform(bool snap_to_pixel= false);

	void MoveMouseToLocalPosition(Vec2f position);

public:
	Pane(Vec2f offset, Vec2f size);
	Pane();
};


class QuadPane : public Pane
{
	bool use_depth= false;

protected:
	virtual ShaderProgram * GetShaderProgram()= 0;
	virtual void UploadShaderUniforms()= 0;

	void UseDepth();
	void DontUseDepth();
	
public:
	virtual void Draw();
};

//For classes that spit out a texture during their Draw()
class TexturePane : public QuadPane
{
	ShaderProgram * GetShaderProgram();
	void UploadShaderUniforms();

protected:
	virtual Texture GetTexture()= 0;
	virtual Transform GetTextureTransform();

public:
	virtual void Draw();
};

//For simply displaying an image
class ImagePane : public TexturePane
{
	Texture texture;

protected:
	virtual Texture GetTexture();
	virtual Transform GetTextureTransform();

public:
	ImagePane(ColorImage image);
	ImagePane(Texture texture);
	ImagePane(string image_filename);
	ImagePane();

	void SetImage(ColorImage image);
	void SetImage(Texture texture);
	void SetImage(string image_filename);
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

	string GetText();
	void SetText(string text, Color color, Align align= Align::Center);
	void SetText(string text);

	virtual void Draw();
};

template<class T>
class MouseFollowingParcel : public WrappedParcel<T>, public Pane
{
public:
	MouseFollowingParcel(Interface *sender, T *item, Vec2f size, Pane *pane)
		: WrappedParcel<T>(sender, item, pane)
	{
		MakeOrphan();

		AddComponent(pane);

		Size= size;
	}

	virtual void MouseMotion(Vec2f mouse_position, Vec2f mouse_displacement)
	{
		Offset= mouse_position- Size.Get()/ 2;
	}
};

#endif