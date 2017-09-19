#include "Button.h"
#include "Drawables.h"

#include <windows.h>
#include "MicrosoftPollutionCleaner.h"


Pane * Button::GetContent()
{
	return content;
}

Button::Button(string ninepatch_filename, Roundness roundness, Pane *content)
	: Ninepatch(ninepatch_filename, roundness), cycle(0.25f)
{
	SetContent(content);
}

void Button::MouseIn()
{
	cycle.Start();

	Ninepatch::MouseIn();
}

void Button::MouseOut()
{
	cycle.Stop();

	Ninepatch::MouseOut();
}

void Button::SetContent(Pane *content_)
{
	if(content_== nullptr)
		return;

	if(content!= nullptr)
		RemoveComponent(content);

	content= content_;
	content->Offset= Vec2f(0.1f, 0.1f);
	content->Size= Vec2f(0.8f, 0.8f);
	AddComponent(content);
}

void Button::MouseLeftClick()
{
	SpawnInterfaceEvent(new ButtonInterfaceEvent(this));
}

void Button::Draw()
{
	Vec2f size= Size;
	Vec2f offset= Offset;
	Size= size* (1.0f+ 0.1f* cycle.GetMoment());
	Offset-= (Size- size)/ 2;

	Ninepatch::Draw();

	Size= size;
	Offset= offset;
}


Button::ButtonInterfaceEvent::ButtonInterfaceEvent(Button *button_)
	: InterfaceEvent(button_)
{
	button= button_;
}

Button * Button::ButtonInterfaceEvent::GetButton()
{
	return button;
}


TextPane * TextButton::GetTextPane()
{
	return &text_pane;
}

string TextButton::GetText()
{
	return text_pane.GetText();
}

void TextButton::SetText(string text)
{
	text_pane.SetText(text);
}

TextButton::TextButton(string ninepatch_filename, string text, Color font_color, Roundness roundness)
	: text_pane(text, font_color), Button(ninepatch_filename, roundness)
{
	SetContent(&text_pane);
}

void TextButton::Draw()
{
	text_pane.LockFontSize();
	Button::Draw();
	text_pane.UnlockFontSize();
}


VisibilityToggleButton::VisibilityToggleButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void VisibilityToggleButton::MouseLeftClick()
{
	if(drawable->IsHidden())
		drawable->Show();
	else
		drawable->Hide();

	Button::MouseLeftClick();
}


ShowButton::ShowButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void ShowButton::MouseLeftClick()
{
	drawable->Show();

	Button::MouseLeftClick();
}


HideButton::HideButton(string ninepatch_filename, string text, Color text_color, Drawable *drawable_, Roundness roundness)
	: TextButton(ninepatch_filename, text, text_color, roundness)
{
	drawable= drawable_;
}

void HideButton::MouseLeftClick()
{
	drawable->Hide();

	Button::MouseLeftClick();
}


ToggleButton::ToggleButton(string ninepatch_filename, Roundness roundness, Color on_color_, Color off_color_, bool is_on)
	: Button(ninepatch_filename, roundness)
{
	on_color= on_color_;
	off_color= off_color_;

	if(is_on)
		TurnOn();
	else
		TurnOff();
}

ToggleButton::ToggleButton()
	: Button("nine.png")
{
	on_color= Color::Green;
	off_color= Color::Red;

	TurnOff();
}

void ToggleButton::TurnOn()
{
	is_on= true;

	SetTint(on_color);
}

void ToggleButton::TurnOff()
{
	is_on= false;

	SetTint(off_color);
}

bool ToggleButton::IsOn()
{
	return is_on;
}



void ToggleButton::MouseLeftClick()
{
	if(is_on)
		TurnOff();
	else
		TurnOn();

	Button::MouseLeftClick();
}


SelectFileButton::SelectFileButton(string initial_filepath, vector<string> file_extensions_)
	: TextButton("nine.png", Utility::GetFilename(initial_filepath), Color::Black)
{
	filepath= initial_filepath;
	file_extensions= file_extensions_;
}

string SelectFileButton::GetFilepath()
{
	return filepath;
}

string SelectFileButton::GetFilename()
{
	return Utility::GetFilename(GetFilepath());
}

void SelectFileButton::SetFilepath(string filepath_)
{
	filepath= filepath_;

	SetText(GetFilename());
}

//http://www.cplusplus.com/forum/windows/169960/
void SelectFileButton::MouseLeftClick()
{
	//"OBJ Files\0*.obj\0Any File\0*.*\0";

	string extension_filter= "";
	for(string extension : file_extensions)
	{
		extension_filter+= extension+ " Files";
		extension_filter.push_back('\0');
		extension_filter+= "*."+ extension;
		extension_filter.push_back('\0');
	}
	extension_filter+= string("Any File\0*.*\0", 13);

	char filepath_[1024];
	OPENFILENAME configuration;
    ZeroMemory(&filepath_, sizeof(filepath_));
    ZeroMemory(&configuration, sizeof(configuration));
    configuration.lStructSize= sizeof(configuration);
    configuration.hwndOwner= NULL;
    configuration.lpstrFilter= extension_filter.c_str();
    configuration.lpstrFile= filepath_;
    configuration.nMaxFile= MAX_PATH;
    configuration.lpstrTitle= "Select an OBJ file";
    configuration.Flags= OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;   

	if(GetOpenFileNameA(&configuration))
	{
		filepath= filepath_;
		SetText(GetFilename());

		SpawnInterfaceEvent(new SelectFileButtonInterfaceEvent(this, filepath));
	}
	else
	{
		switch (CommDlgExtendedError())
		{
			case CDERR_DIALOGFAILURE   : std::cout << "CDERR_DIALOGFAILURE\n";   break;
			case CDERR_FINDRESFAILURE  : std::cout << "CDERR_FINDRESFAILURE\n";  break;
			case CDERR_INITIALIZATION  : std::cout << "CDERR_INITIALIZATION\n";  break;
			case CDERR_LOADRESFAILURE  : std::cout << "CDERR_LOADRESFAILURE\n";  break;
			case CDERR_LOADSTRFAILURE  : std::cout << "CDERR_LOADSTRFAILURE\n";  break;
			case CDERR_LOCKRESFAILURE  : std::cout << "CDERR_LOCKRESFAILURE\n";  break;
			case CDERR_MEMALLOCFAILURE : std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
			case CDERR_MEMLOCKFAILURE  : std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
			case CDERR_NOHINSTANCE     : std::cout << "CDERR_NOHINSTANCE\n";     break;
			case CDERR_NOHOOK          : std::cout << "CDERR_NOHOOK\n";          break;
			case CDERR_NOTEMPLATE      : std::cout << "CDERR_NOTEMPLATE\n";      break;
			case CDERR_STRUCTSIZE      : std::cout << "CDERR_STRUCTSIZE\n";      break;
			case FNERR_BUFFERTOOSMALL  : std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
			case FNERR_INVALIDFILENAME : std::cout << "FNERR_INVALIDFILENAME\n"; break;
			case FNERR_SUBCLASSFAILURE : std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
			default                    : std::cout << "You cancelled.\n";
		}
	}

	TextButton::MouseLeftClick();
}


SelectFileButton::SelectFileButtonInterfaceEvent::SelectFileButtonInterfaceEvent(SelectFileButton *button, string filepath_)
	: ButtonInterfaceEvent(button)
{
	filepath= filepath_;
}

string SelectFileButton::SelectFileButtonInterfaceEvent::GetFilepath()
{
	return filepath;
}

string SelectFileButton::SelectFileButtonInterfaceEvent::GetFilename()
{
	return Utility::GetFilename(filepath);
}


ImageButton::ImageButton(string image_filename)
	: Button("nine.png", Roundness::Normal, nullptr), image_pane(image_filename)
{
	SetContent(&image_pane);
}
