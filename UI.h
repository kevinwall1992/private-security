#ifndef PS_UI
#define PS_UI

#include "Pane.h"
#include "Gizmos.h"
#include "Button.h"
#include "Menu.h"
#include "Dialog.h"
#include "ListPane.h"
#include "IconCamera.h"
#include "Cycle.h"
#include "Savable.h"
#include "Textbox.h"
#include "Dropdown.h"
#include "RasterCamera.h"


//High level user interface classes


class AboutDialog : public Dialog
{
public:
	AboutDialog();
};


class PauseMenu : public Menu
{
	ListPane button_list_pane;

	TextButton about_button;
	TextButton quit_button;
	TextButton resume_button;
	TextButton save_button;
	TextButton load_button;

	TextPane::Group button_group;

	string save_load_filepath;

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	PauseMenu();

	void SetSaveLoadFilepath(string save_load_filepath);

	void AddButton(Button *button);
};

class SaveFileDialog : public Menu
{
	ListPane list_pane;

	Ninepatch background;
	TextPane prompt_text;
	Textbox filename_textbox;
	TextButton save_button;

	string filepath= "";

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	SaveFileDialog(string initial_filepath, Interface *interface_event_recipient);

	string GetFilepath();
	string GetFilename();

	class FilepathSelectedEvent : public InterfaceEvent
	{
	public:
		FilepathSelectedEvent(SaveFileDialog *save_file_dialog);

		string GetFilepath();
		string GetFilename();
	};
};

class LoadFileDialog : public Menu
{
	ListPane list_pane;

	SelectFileButton select_file_button;
	TextButton load_button;

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	LoadFileDialog(string initial_filepath, Interface *interface_event_recipient, vector<string> file_extensions);

	string GetFilepath();
	string GetFilename();

	class FilepathSelectedEvent : public InterfaceEvent
	{
	public:
		FilepathSelectedEvent(LoadFileDialog *load_file_dialog);

		string GetFilepath();
		string GetFilename();
	};
};

class IconPane : public TexturePane
{
	static IconCamera *icon_camera;

	MeshProp *mesh_prop;

	Scene scene;
	SmoothCycle cycle;

	float scale;

public:
	IconPane(Mesh *mesh, float scale= 1.0f);

	float GetScale();
	void SetScale(float scale);

	Mesh * GetMesh();
	void SetMesh(Mesh *mesh);

	virtual void MouseIn();
	virtual void MouseOut();

	virtual Texture GetTexture();

	virtual void Draw();
};

class ObjectIconButton : public Button
{
	IconPane icon_pane;

	Interface *object_owner;
	Object *object_;

public:
	ObjectIconButton(string ninepatch_filename, Roundness roundness, Object *object_, Interface *object_owner_)
		: Button(ninepatch_filename, roundness, nullptr), icon_pane(nullptr, 2)
	{
		object_owner= object_owner_;

		SetObject(object_);
		SetContent(&icon_pane);
	}

	ObjectIconButton(Object *object_, Interface *object_owner_)
		: Button("nine.png", Roundness::Normal, nullptr), icon_pane(nullptr, 2)
	{
		object_owner= object_owner_;

		SetObject(object_);
		SetContent(&icon_pane);
		GetContent()->Size= Vec2f(0.9f, 0.9f);
		GetContent()->Offset= Vec2f(0.05f, 0.05f);
	}

	Object * GetObject()
	{
		return object_;
	}

	void SetObject(Object *object_)
	{
		this->object_= object_;

		if(Utility::IsType<Actor>(object_))
			icon_pane.SetScale(1);
		else
			icon_pane.SetScale(2);

		if(object_!= nullptr)
			icon_pane.SetMesh(this->object_->GetIconMesh());
		else
			icon_pane.SetMesh(nullptr);
	}

	virtual void MouseLeftClick()
	{
		Button::MouseLeftClick();
	}

	virtual void Dragged()
	{
		if(object_!= nullptr && object_owner!= nullptr)
			SendParcel(new MouseFollowingParcel<Object>(object_owner, object_, Vec2f(0.1f, 0.1f), new ObjectIconButton(object_, nullptr)));
	}

	virtual void Draw()
	{
		if(object_!= nullptr && icon_pane.GetMesh()!= object_->GetIconMesh())
			icon_pane.SetMesh(object_->GetIconMesh());

		Button::Draw();
	}
};

class ToolbeltSlot : public Button
{
	Item *item= nullptr;

public:
	ToolbeltSlot();

	void SetItem(Item *item);
	Item * GetItem();
};

class Toolbelt : public ListPane
{
	ToolbeltSlot slots[10];
	Actor *actor= nullptr;

public:
	Toolbelt();

	void SetActor(Actor *actor);

	virtual void Draw();
};


class _3DInterfacePane : public CameraPane
{
	CameraGizmo *camera_gizmo;

	int level= 0;

	Tile *hovered_tile;
	Tile *selected_tile_left;
	Tile *selected_tile_right;
	Actor *selected_actor_left= nullptr;
	Actor *selected_actor_right= nullptr;
	std::map<Tile *, Color> highlighted_tiles;

	bool highlight_selected_tile= false;

	bool selection_is_locked= false;

	VAO tile_frame_vao, movement_graph_vao;
	GLuint movement_graph_vertex_buffer_handle;
	static float tile_frame_vertices[24* 3];

protected:
	Tile * GetTile(Ray ray);

public:
	_3DInterfacePane(Camera *camera);

	Tile * GetHoveredTile();
	Tile * GetSelectedTile(bool mouse_left= true);
	Actor * GetSelectedActor(bool mouse_left= true);

	void AddHighlightedTile(Tile *tile, Color highlight_color);
	void ClearHighlightedTiles();

	void HighlightSelectedTile();
	void DontHighlightSelectedTile();

	void LockSelection();
	void UnlockSelection();

	virtual void KeyUp(ButtonType button);

	virtual void MouseLeftUpRay(Ray ray);
	virtual void MouseRightUpRay(Ray ray);
	virtual void MouseMotionRay(Ray ray, Ray last_ray);
	virtual void MouseScroll(int scroll_amount);

	virtual void MouseLeftTileSelected(Tile *tile);
	virtual void MouseRightTileSelected(Tile *tile);
	virtual void MouseLeftActorSelected(Actor *actor);
	virtual void MouseRightActorSelected(Actor *actor);

	virtual void Draw();


	class TileSelectedEvent : public InterfaceEvent
	{
		Tile *selected_tile;

	public:
		TileSelectedEvent(_3DInterfacePane *_3d_interface_pane, Tile *selected_tile);

		Tile * GetSelectedTile();
	};

	class TileHoveredEvent : public InterfaceEvent
	{
		Tile *hovered_tile;

	public:
		TileHoveredEvent(_3DInterfacePane *_3d_interface_pane, Tile *hovered_tile);

		Tile * GetHoveredTile();
	};
};


class TacticalInterface : public _3DInterfacePane
{
	Toolbelt toolbelt;

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	TacticalInterface(Camera *camera);

	virtual void MouseRightUpRay(Ray ray);
	virtual void MouseLeftActorSelected(Actor *actor);

	virtual void Draw();
};

class MapEditor : public Pane
{
	class DetailPaneBrowser;
	class DetailPane : public Pane
	{
		DetailPaneBrowser *detail_pane_browser= nullptr;

	protected:
		DetailPane();

		DetailPaneBrowser * GetDetailPaneBrowser();
		void SetDetailPaneBrowser(DetailPaneBrowser *detail_pane_browser);

	public:
		void Close();

		friend class DetailPaneBrowser;
	};

	class DetailPaneBrowser : public Pane
	{
		std::stack<DetailPane *> detail_pane_stack;

	public:
		void Open(DetailPane *detail_pane);
		void Back();

		DetailPane * GetCurrentDetailPane();
	};

	class TileDetailPane : public DetailPane
	{
		Tile *tile= nullptr;

		Ninepatch background;

		ObjectIconButton actor_icon_button;
		ListPane item_list_pane, furniture_list_pane;

		void FillListPanes();

		void ActorClicked(Actor *actor_object);
		void ItemClicked(Item *item_object);
		void FurnitureClicked(Furniture *furniture_object);

		static void ObjectClicked(void *tile_detail_pane, Object *object_);

	protected:
			void HandleInterfaceEvent(InterfaceEvent *interface_event);

	public:
		TileDetailPane(Tile *tile);

		void RecieveParcel(Parcel *parcel);
		void RelinquishParcel(Parcel *parcel, bool must_transfer_ownership);
	};

	class ObjectDetailPane : public DetailPane
	{
		using DetailPane::GetDetailPaneBrowser;

		class Element : public Ninepatch
		{
		public:
			Element();
		};

		class EntityElement : public Element
		{
			class MeshPane : public ListPane
			{
				EntityElement *entity_element;

				Textbox name_textbox;
				SelectFileButton filename_button;
				Dropdown element_name_dropdown;

				void FillElementNameDropdown();

			protected:
				void HandleInterfaceEvent(InterfaceEvent *interface_event);

			public:
				MeshPane(EntityElement *entity_element, string name);
			};

			Entity *entity;

			ListPane mesh_list_pane;

		public:
			EntityElement(Entity *entity);
		};

		class ActorElement : public Element
		{
			Actor *actor;

			ListPane inventory_list_pane;

			ObjectDetailPane *object_detail_pane;

			void FillInventoryPane();

		protected:
			void HandleInterfaceEvent(InterfaceEvent *interface_event);

		public:
			ActorElement(Actor *actor, ObjectDetailPane *object_detail_pane);

			void RecieveParcel(Parcel *parcel);
			void RelinquishParcel(Parcel *parcel, bool must_transfer_ownership);
		};

		class ItemElement : public Element
		{	
			Item *item;

		public:
			ItemElement(Item *item);
		};

		class FurnitureElement : public Element
		{
			Furniture *furniture;

			ToggleButton direction_buttons[8];

			Direction::Enum GetDirection(ToggleButton *direction_button);

			void UpdateFurnitureObject();
			void UpdateDirectionButtonState();

		protected:
			void HandleInterfaceEvent(InterfaceEvent *interface_event);

		public:
			FurnitureElement(Furniture *furniture);

			virtual void KeyUp(ButtonType button);
		};

		ListPane element_list_pane;

		Object *object_= nullptr;

	public:
		ObjectDetailPane(Object *object_);
	};

	class Toolbox : public GridPane, public Savable
	{
		Object *active_object= nullptr;

		DetailPaneBrowser *detail_pane_browser;
		Button new_object_button;

		void ObjectDragged(Object *object_);

	protected:
		void HandleInterfaceEvent(InterfaceEvent *interface_event);

	public:
		Toolbox(string filepath, DetailPaneBrowser *detail_pane_browser);

		void Save(string filepath);
		void Load(string filepath);

		void AddObject(Object *object_);

		Object * GetActiveObject();
		void SetActiveObject(Object *object_);

		void RecieveParcel(Parcel *parcel);
		void RelinquishParcel(Parcel *parcel, bool must_transfer_ownership);

		virtual void Draw();
	};

	class NewObjectDetailPane : public DetailPane
	{
		Dropdown object_type_dropdown;

		Toolbox *toolbox;

	protected:
		void HandleInterfaceEvent(InterfaceEvent *interface_event);

	public:
		NewObjectDetailPane(Toolbox *toolbox);
	};

	class TrashPane : public Pane
	{
		ImageButton trash_button;
		ListPane list_pane;

		void AddObject(Object *object_);

	public:
		TrashPane();

		void RecieveParcel(Parcel *parcel);
		void RelinquishParcel(Parcel *parcel, bool must_transfer_ownership);

		void Focus();
		void Defocus();

		void MouseLeftClick();
	};

	_3DInterfacePane _3d_interface_pane;

	DetailPaneBrowser detail_pane_browser;
	Toolbox toolbox;
	TrashPane trash_pane;

	Tile *preview_tile= nullptr;
	Object *preview_object= nullptr;
	float preview_rotation= 0;
	bool is_rotating_preview= false;
	Vec2f mouse_position_before_preview_rotation;

	bool PutObjectIntoTile(Object *object_, Tile *tile);
	void RemoveObjectFromTile(Object *object_, Tile *tile);

	void ClearPreviewObject();

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	MapEditor(Camera *camera);

	void SetScene(Scene *scene);

	void SaveToolbox(string filepath);
	void LoadToolbox(string filepath);

	virtual void KeyUp(ButtonType button);

	virtual void MouseRightClick();
	virtual void MouseRightDown(Vec2f mouse_position);
	virtual void MouseRightDrag(Vec2f mouse_position, Vec2f mouse_displacement);

	virtual void Draw();

	virtual void HandleEvent(Event *event_);
};

class DevelopmentInterface : public Pane
{
	RasterCamera camera;
	Scene scene;

	TacticalInterface tactical_interface;
	MapEditor map_editor;

	TextButton mode_toggle_button;
	bool tactical_interface_is_active= false;

	string tactical_interface_toggle_text= "Map Editor";
	string map_editor_toggle_text= "Tactical Interface";

protected:
	void HandleInterfaceEvent(InterfaceEvent *interface_event);

public:
	DevelopmentInterface();

	virtual void KeyUp(ButtonType button);
};

#endif
