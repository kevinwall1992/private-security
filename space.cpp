#include "Space.h"
#include "GameSystem.h"


Vec3i Space::IndexToPosition(int index)
{
	int column= index% column_count;
	int row= (index% (column_count* row_count))/ column_count;
	int level= index/ (column_count* row_count);

	return Vec3i(column, level, row);
}

int Space::GetTileCount()
{
	return column_count* row_count* level_count;
}

void Space::DeleteContents()
{
	if(tiles== nullptr)
		return;

	for(int i= 0; i< GetTileCount(); i++)
	{
		Tile *tile= &tiles[i];

		if(tile->HasActor())
			delete tile->RemoveActor();

		for(Item *item: tile->GetItems())
			delete tile->RemoveItem(item);

		vector<Furniture *> furnitures= tile->GetFurnitures();
		for(Furniture *furniture : furnitures)
			delete tile->RemoveFurniture(furniture);
	}

	delete []tiles;
}

void Space::Resize(int column_count_, int row_count_, int level_count_)
{
	DeleteContents();

	column_count= column_count_;
	row_count= row_count_;
	level_count= level_count_;

	tiles= new Tile[column_count* row_count* level_count];

	for(int i= 0; i< GetTileCount(); i++)
		tiles[i].SetSpace(this);
}

Space::Space(int column_count, int row_count, int level_count)
{
	Resize(column_count, row_count, level_count);
}

Space::Space()
{
}

Space::Space(string filepath)
{
	Load(filepath);
}

Tile * Space::GetTile(int column, int row, int level)
{
	if((column< 0 || column>= column_count) || (row< 0 || row>= row_count) || (level< 0 || level>= level_count))
		return nullptr;

	return &tiles[level* column_count* row_count+ row* column_count+ column];
}

Tile * Space::GetTile(Vec3i position)
{
	return GetTile(position.x, position.z, position.y);
}

Vec3i Space::GetPosition(Tile *tile)
{
	for(int i= 0; i< GetTileCount(); i++)
		if(&tiles[i]== tile)
			return IndexToPosition(i);

	return Vec3i(-1, -1, -1);
}

Vec3i Space::GetPosition(Object *object)
{
	for(int i= 0; i< GetTileCount(); i++)
		if(tiles[i].Contains(object))
			return IndexToPosition(i);

	return Vec3i(-1, -1, -1);
}

Vec3i Space::GetSize()
{
	return Vec3i(column_count, level_count, row_count);
}

vector<Perceptible*> Space::GetPerceptibles()
{
	vector<Perceptible *> perceptibles;

	for(int i= 0; i< GetTileCount(); i++)
		perceptibles.push_back(&tiles[i]);

	return perceptibles;
}

void Space::Step(Chronons chronons)
{
	for(int i= 0; i< GetTileCount(); i++)
		tiles[i].Step(chronons);
}

void Space::Save(string filepath)
{
	Savable::Save(filepath);

	TiXmlDocument xml_document;

	TiXmlElement *xml_space= new TiXmlElement("map");

	xml_space->LinkEndChild(Utility::XML::EncodeVector(Vec3i(column_count, row_count, level_count), "size"));

	vector<std::pair<Object *, Vec3i>> objects;

	for(int i= 0; i< GetTileCount(); i++)
	{
		Tile *tile= &tiles[i];
		Vec3i position= IndexToPosition(i);

		if(tile->HasActor())
			objects.push_back(std::pair<Actor *, Vec3i>(tile->GetActor(), position));

		vector<Item *> tile_items= tile->GetItems();
		for(unsigned int j= 0; j< tile_items.size(); j++)
			objects.push_back(std::pair<Item *, Vec3i>(tile_items[j], position));

		vector<Furniture *> tile_furnitures= tile->GetFurnitures();
		for(unsigned int j= 0; j< tile_furnitures.size(); j++)
			objects.push_back(std::pair<Furniture *, Vec3i>(tile_furnitures[j], position));
	}

	TiXmlElement *xml_list= new TiXmlElement("objects");
	for(std::pair<Object *, Vec3i> object_pair: objects)
	{
		Object *object= object_pair.first;
		Vec3i position= object_pair.second;

		TiXmlElement *xml_object= object->EncodeXML();
		
		xml_object->LinkEndChild(Utility::XML::EncodeVector(position, "position"));

		xml_list->LinkEndChild(xml_object);
	}
	xml_space->LinkEndChild(xml_list);
		
	xml_document.LinkEndChild(xml_space);
	xml_document.SaveFile(filepath.c_str());
}

void Space::Load(string filepath)
{
	Savable::Load(filepath);

	TiXmlDocument xml_document;
	xml_document.LoadFile(filepath.c_str());

	TiXmlElement *xml_space= xml_document.FirstChildElement("map");

	Vec3i size= Utility::XML::DecodeVec3i(xml_space->FirstChildElement("size"));
	Resize(size.x, size.y, size.z);

	TiXmlElement *xml_object= xml_space->FirstChildElement("objects")->FirstChildElement();
	while(xml_object!= nullptr)
	{
		Object *object_= Encodable::DecodeXML<Object>(xml_object);
		
		Vec3i position= Utility::XML::DecodeVec3i(xml_object->FirstChildElement("position"));

		if(Utility::IsType<Actor>(object_))
			GetTile(position)->PutActor(dynamic_cast<Actor *>(object_));
		else if(Utility::IsType<Item>(object_))
			GetTile(position)->PutItem(dynamic_cast<Item *>(object_));
		else if(Utility::IsType<Furniture>(object_))
			GetTile(position)->PutFurniture(dynamic_cast<Furniture *>(object_));

		xml_object= xml_object->NextSiblingElement();
	}
}
