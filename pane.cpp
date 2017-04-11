#include "Pane.h"
#include "Framebuffer.h"
#include "Viewport.h"


void Pane::AddComponent(Pane *pane)
{
	Interface::AddComponent(pane);

	pane->SetParent(this);
}

void Pane::AddComponent(Interface *interface_)
{
	Interface::AddComponent(interface_);
}

void Pane::AddComponent(Drawable *drawable)
{
	Interface::AddComponent(drawable);
}

void Pane::AddComponent(Gizmo *gizmo)
{
	Interface::AddComponent(gizmo);
}

Vec2i Pane::LocalPositionToPixelPosition(Vec2f position)
{
	return LocalPositionToGlobalPosition(position)* System::graphics.GetScreenSize();
}

Vec2i Pane::LocalSizeToPixelSize(Vec2f size)
{
	return LocalSizeToGlobalSize(size)* System::graphics.GetScreenSize();
}

Vec2f Pane::PixelPositionToLocalPosition(Vec2i position)
{
	return GlobalPositionToLocalPosition((Vec2f)position/ System::graphics.GetScreenSize());
}

Vec2f Pane::PixelSizeToLocalSize(Vec2i size)
{
	return GlobalSizeToLocalSize((Vec2f)size/ System::graphics.GetScreenSize());
}

Vec2f Pane::ScreenPositionToLocalPosition(Vec2f position, bool snap_to_pixel)
{
	Vec2i screen_size= System::graphics.GetScreenSize();

	position*= screen_size;
	if(snap_to_pixel)
		position= (Vec2i)position;

	return GlobalPositionToLocalPosition(position/ screen_size);
}

Vec2f Pane::ScreenSizeToLocalSize(Vec2f size, bool snap_to_pixel)
{
	Vec2i screen_size= System::graphics.GetScreenSize();

	size*= screen_size;
	if(snap_to_pixel)
		size= (Vec2i)size;

	return GlobalSizeToLocalSize(size/ screen_size);
}

Vec2f Pane::LocalPositionToScreenPosition(Vec2f position, bool snap_to_pixel)
{
	Vec2i screen_size= System::graphics.GetScreenSize();

	position= LocalPositionToGlobalPosition(position)* screen_size;
	if(snap_to_pixel)
		position= (Vec2i)position;
	position/= screen_size;

	return position;
}

Vec2f Pane::LocalSizeToScreenSize(Vec2f size, bool snap_to_pixel)
{
	Vec2i screen_size= System::graphics.GetScreenSize();

	size= LocalSizeToGlobalSize(size)* screen_size;
	if(snap_to_pixel)
		size= (Vec2i)size;
	size/= screen_size;

	return size;
}

Vec2i Pane::GetPixelOffset()
{
	return LocalPositionToPixelPosition(GetLocalOffset());
}

Vec2i Pane::GetPixelSize()
{
	return LocalSizeToPixelSize(GetLocalSize());
}

float Pane::GetAspectRatio()
{
	return Math::GetAspectRatio(LocalSizeToScreenSize(GetLocalSize())* System::graphics.GetScreenSize());
}

bool Pane::IsInside(Vec2f point)
{
	point= GlobalPositionToLocalPosition(point);

	return (point.x>= 0 && point.y>= 0) && (point.x< 1 && point.y< 1);
}

Pane::Pane(Vec2f offset, Vec2f size)
	: TreeFrame(offset, size)
{

}

Pane::Pane()
{
	Offset= Vec2f(0.0f, 0.0f);
	Size= Vec2f(1.0f, 1.0f);
}


Transform QuadPane::GetQuadTransform(bool snap_to_pixel)
{
	Vec2f screen_offset= LocalPositionToScreenPosition(GetLocalOffset(), snap_to_pixel);
	Vec2f screen_size= LocalSizeToScreenSize(GetLocalSize(), snap_to_pixel);

	return Transform().Translate(Vec3f(1, 1, 0)).Scale(screen_size.Push()/ 2).Translate(screen_offset.Push()).Scale(2).Translate(Vec3f(-1, -1, 0));
}

void QuadPane::UseDepth()
{
	use_depth= true;
}

void QuadPane::DontUseDepth()
{
	use_depth= false;
}

void QuadPane::Draw()
{
	GetShaderProgram()->Use();
	UploadShaderUniforms();

	if(!use_depth)
	{
		glDisable(GL_DEPTH_TEST);
		RasterizeFullScreenQuad();
		glEnable(GL_DEPTH_TEST);
	}
	else
		RasterizeFullScreenQuad();

	Pane::Draw();
}


ShaderProgram * TexturePane::GetShaderProgram()
{
	return ShaderProgram::Retrieve("quad.program");
}

void TexturePane::UploadShaderUniforms()
{
	GetShaderProgram()->SetUniformMatrix4x4f("transform", GetQuadTransform(true));
	GetShaderProgram()->SetUniformMatrix4x4f("texture_transform", Transform());
}

void TexturePane::Draw()
{
	GetTexture().BindToIndex(0);

	QuadPane::Draw();
}


Camera * CameraPane::GetCamera()
{
	return camera;
}

Texture CameraPane::GetTexture()
{
	Viewport::Push();
	Framebuffer framebuffer= Framebuffer::GetCurrentFramebuffer();

	PhotoBook photo_book= GetCamera()->TakePhotos(*scene, GetPixelSize(), (Photo::Type)(Photo::Type::FullColor | Photo::Type::Depth));
	Texture color_texture= photo_book[Photo::Type::FullColor].GetTexture();
	depth_texture= photo_book[Photo::Type::Depth].GetTexture();

	framebuffer.Bind();
	Viewport::Pop();

	return color_texture;
}

Ray CameraPane::GetRay(Vec2f mouse_position)
{
	mouse_position= ScreenPositionToLocalPosition(mouse_position);

	Vec3f forward= camera->GetForward();

	Vec3f view_plane_u= camera->GetRight()* tan(camera->FOV/ 2);
	Vec3f view_plane_v= camera->GetUp()* tan(camera->FOV/ 2);
	
	mouse_position= mouse_position* 2- Vec2f(1, 1);

	if(camera->IsOrthographic())
		return Ray(camera->Position+ (view_plane_u.Normalized()* mouse_position.x+ view_plane_v.Normalized()* mouse_position.y)* (camera->GetOrthographicHorizontalSize()/ 2.0f), forward);
	else
		return Ray(camera->Position, forward+ view_plane_u* mouse_position.x+ view_plane_v* mouse_position.y);
}

void CameraPane::AttachDepth()
{
	Framebuffer draw_framebuffer= Framebuffer::GetCurrentFramebuffer();
	Framebuffer framebuffer;

	framebuffer.AttachDepthTexture(depth_texture);
	framebuffer.BlitOnto(draw_framebuffer, false, true, GetPixelOffset(), GetPixelSize());
	framebuffer.Free();

	draw_framebuffer.Bind();
}

void CameraPane::HandleMouseMotionEvent(MouseMotionEvent *mouse_motion_event)
{
	TexturePane::HandleMouseMotionEvent(mouse_motion_event);

	Vec2f last_mouse_position= mouse_motion_event->GetMousePosition()- mouse_motion_event->GetMouseDisplacement();
	MouseMotionRay(GetRay(mouse_motion_event->GetMousePosition()), GetRay(last_mouse_position));
}

void CameraPane::HandleMouseButtonEvent(MouseButtonEvent *mouse_button_event)
{
	TexturePane::HandleMouseButtonEvent(mouse_button_event);

	switch(mouse_button_event->GetMouseButton())
	{
	case MouseButtonType::Left:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseLeftUpRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::Down:			MouseLeftDownRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::DoubleClick:	MouseLeftDoubleClickRay(GetRay(mouse_button_event->GetMousePosition())); break;
		default: break;
		}
		break;

	case MouseButtonType::Middle:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseMiddleUpRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::Down:			MouseMiddleDownRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::DoubleClick:	MouseMiddleDoubleClickRay(GetRay(mouse_button_event->GetMousePosition())); break;
		default: break;
		}
		break;

	case MouseButtonType::Right:
		switch(mouse_button_event->GetType())
		{
		case EventType::Up:				MouseRightUpRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::Down:			MouseRightDownRay(GetRay(mouse_button_event->GetMousePosition())); break;
		case EventType::DoubleClick:	MouseRightDoubleClickRay(GetRay(mouse_button_event->GetMousePosition())); break;
		default: break;
		}
		break;

	default: break;
	}
}

void CameraPane::MouseLeftUpRay(Ray ray) {}
void CameraPane::MouseMiddleUpRay(Ray ray) {}
void CameraPane::MouseRightUpRay(Ray ray) {}
void CameraPane::MouseLeftDownRay(Ray ray) {}
void CameraPane::MouseMiddleDownRay(Ray ray) {}
void CameraPane::MouseRightDownRay(Ray ray) {}
void CameraPane::MouseLeftDoubleClickRay(Ray ray) {}
void CameraPane::MouseMiddleDoubleClickRay(Ray ray) {}
void CameraPane::MouseRightDoubleClickRay(Ray ray) {}
void CameraPane::MouseMotionRay(Ray ray, Ray last_ray) {}

CameraPane::CameraPane(Camera *camera_)
{
	camera= camera_;
}

void CameraPane::SetScene(Scene *scene_)
{
	scene= scene_;
}


int TextPane::Group::GetMaximumFontSize()
{
	if(text_panes.size()== 0)
		return 0;

	int maximum_font_size= text_panes[0]->GetLocalMaximumFontSize();
	for(unsigned int i= 1; i< text_panes.size(); i++)
	{
		int local_maximum_font_size= text_panes[i]->GetLocalMaximumFontSize();

		if(local_maximum_font_size< maximum_font_size)
			maximum_font_size= local_maximum_font_size;
	}

	return maximum_font_size;
}

void TextPane::Group::AddTextPane(TextPane *text_pane)
{
	text_panes.push_back(text_pane);
	text_pane->SetGroup(this);
}

void TextPane::GenerateTextDrawables()
{
	for(TextDrawable *text_drawable : text_drawables)
		delete text_drawable;

	if(!IsFontSizeLocked())
		font_size= GetMaximumFontSize();

	vector<string> lines= Utility::SplitString(text);
	for(string line : lines)
		text_drawables.push_back(new TextDrawable(line, font_size, color));
}

int TextPane::GetLocalMaximumFontSize()
{
	if(IsFontSizeLocked())
		return font_size;
	else
		return Font::Default->GetMaximumFontSize(text, GetPixelSize());
}

int TextPane::GetMaximumFontSize()
{
	return group== nullptr ? GetLocalMaximumFontSize() : group->GetMaximumFontSize();
}

void TextPane::SetGroup(Group *group_)
{
	group= group_;
}

TextPane::TextPane(string text, Color color, Align align)
{
	SetText(text, color, align);
}

TextPane::TextPane()
{
	text= "";
}

void TextPane::SetFontSize(int font_size_)
{
	font_size= font_size_;

	LockFontSize();
}

void TextPane::LockFontSize()
{
	font_size_locked= true;
}

void TextPane::UnlockFontSize()
{
	font_size_locked= false;
}

bool TextPane::IsFontSizeLocked()
{
	return font_size_locked && font_size!= -1;
}

void TextPane::SetText(string text_, Color color_, Align align_)
{
	text= text_;
	color= color_;
	align= align_;
}

void TextPane::Draw()
{
	if(font_size!= GetMaximumFontSize() || text_drawables.size()== 0)
	{
		if(text== "")
			return;

		GenerateTextDrawables();
	}


	Vec2i pane_pixel_size= LocalSizeToPixelSize(GetLocalSize());
	Vec2i pane_pixel_offset= LocalPositionToPixelPosition(GetLocalOffset());

	for(unsigned int i= 0; i< text_drawables.size(); i++)
	{
		Vec2i line_pixel_size= text_drawables[i]->GetTextureSize();
		Vec2i line_pixel_offset;

		switch(align)
		{
		case TopLeft:
			line_pixel_offset= pane_pixel_offset+ Vec2i(0, LocalPositionToPixelPosition(GetLocalOffset()).y- (i+ 1)* font_size);
			break;

		case TopCenter:
			line_pixel_offset= pane_pixel_offset+ Vec2i((pane_pixel_size.x- line_pixel_size.x)/ 2, LocalSizeToPixelSize(Vec2f(1, 1)).y- (i+ 1)* font_size);
			break;

		case Center:
			line_pixel_offset= pane_pixel_offset+ (pane_pixel_size- line_pixel_size)/ 2;
			break;

		default:
			assert(false && "TextPane::Draw(): Unsupported Align");
			break;
		}

	
		Vec2f text_global_size= (Vec2f)line_pixel_size/ System::graphics.GetScreenSize();
		Vec2f text_global_offset= (Vec2f)line_pixel_offset/ System::graphics.GetScreenSize();

		text_drawables[i]->SetTransform(Transform().Translate(Vec3f(1, 1, 0)).Scale(0.5f).Scale(text_global_size.Push()).Translate(text_global_offset.Push()).Scale(2).Translate(Vec3f(-1, -1, 0)));
		text_drawables[i]->Draw();
	}
}
