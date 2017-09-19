#include "Mesh.h"
#include "Material.h"

#include <map>
#include <fstream>


int Mesh::GetTriangleCount()
{
	return (int)(position_indices.size()/ 3);
}

int Mesh::GetVertexCount()
{
	return (int)(positions.size()/ 3);
}

void Mesh::Clear()
{
	material= nullptr;

	positions.clear();
	texture_coordinates.clear();
	normals.clear();

	position_indices.clear();
	texture_coordinate_indices.clear();
	normal_indices.clear();
}

void BakeSkin(Mesh *mesh, std::map<string, std::map<int, float>> skinning_data)
{
	struct SkinnedVertex
	{
		float first_weight, second_weight;
		int first_bone_index, second_bone_index;

		void AddWeight(float weight, int bone_index)
		{
			if(weight> second_weight)
			{
				if(weight> first_weight)
				{
					second_weight= first_weight;
					second_bone_index= first_bone_index;
					first_weight= weight;
					first_bone_index= bone_index;
				}
				else
				{
					second_weight= weight;
					second_bone_index= bone_index;
				}
			}
		}

		void Normalize()
		{
			float total_weight= first_weight+ second_weight;

			first_weight= first_weight/ total_weight;
			second_weight= second_weight/ total_weight;
		}
	};

	std::map<int, SkinnedVertex> skinned_vertices;
	int bone_count= 0;

	for(auto const &bone_vertex_weight_map_pair : skinning_data)
	{
		mesh->skin.bone_index_map[bone_vertex_weight_map_pair.first]= bone_count;

		for(auto &vertex_weight_pair : bone_vertex_weight_map_pair.second)
			skinned_vertices[vertex_weight_pair.first].AddWeight(vertex_weight_pair.second, bone_count);

		bone_count++;
	}

	mesh->skin.vertex_weights.resize(mesh->GetVertexCount()* 2);
	mesh->skin.vertex_bone_indices.resize(mesh->skin.vertex_weights.size());

	for(auto &skinning_pair : skinned_vertices)
	{
		skinning_pair.second.Normalize();
		mesh->skin.vertex_weights[skinning_pair.first* 2+ 0]= skinning_pair.second.first_weight;
		mesh->skin.vertex_weights[skinning_pair.first* 2+ 1]= skinning_pair.second.second_weight;
		mesh->skin.vertex_bone_indices[skinning_pair.first* 2+ 0]= skinning_pair.second.first_bone_index;
		mesh->skin.vertex_bone_indices[skinning_pair.first* 2+ 1]= skinning_pair.second.second_bone_index;
	}
}

void EnforceUniformIndexOrder(Mesh *mesh)
{
	//return;

	bool no_texture_coordinates= mesh->texture_coordinate_indices.size()== 0;

	typedef std::pair<int, std::pair<int, int>> Triple;//Make dedicated struct
	std::map<Triple, int> combinations;
		
	int combination_count= 0;
	for(unsigned int i= 0; i< mesh->position_indices.size(); i++)
	{
		Triple triple;
		triple.first= mesh->position_indices[i];
		triple.second.first= no_texture_coordinates ? -1 : mesh->texture_coordinate_indices[i];
		triple.second.second= mesh->normal_indices[i];

		if(combinations.find(triple)== combinations.end())
			combinations[triple]= combination_count++;
					
		mesh->position_indices[i]= combinations[triple];
		if(!no_texture_coordinates)
			mesh->texture_coordinate_indices[i]= combinations[triple];
		mesh->normal_indices[i]= combinations[triple];
	}

	vector<float> positions;
	positions.resize(combination_count* 3);
	vector<float> texture_coordinates;
	if(!no_texture_coordinates)
		texture_coordinates.resize(combination_count* 2);
	vector<float> normals;
	normals.resize(combination_count* 3);

	Mesh::Skin skin;
	skin.vertex_weights.resize(combination_count* 2);
	skin.vertex_bone_indices.resize(combination_count* 2);

	for(auto const & pair : combinations)
	{
		Triple triple= pair.first;
		int index= pair.second;

		int position_index= triple.first;
		int texture_coordinate_index= triple.second.first;
		int normal_index= triple.second.second;

		positions[index* 3+ 0]= mesh->positions[position_index* 3+ 0];
		positions[index* 3+ 1]= mesh->positions[position_index* 3+ 1];
		positions[index* 3+ 2]= mesh->positions[position_index* 3+ 2];

		if(!no_texture_coordinates)
		{
			texture_coordinates[index* 2+ 0]= mesh->texture_coordinates[texture_coordinate_index* 2+ 0];
			texture_coordinates[index* 2+ 1]= mesh->texture_coordinates[texture_coordinate_index* 2+ 1];
		}

		normals[index* 3+ 0]= mesh->normals[normal_index* 3+ 0];
		normals[index* 3+ 1]= mesh->normals[normal_index* 3+ 1];
		normals[index* 3+ 2]= mesh->normals[normal_index* 3+ 2];

		skin.vertex_bone_indices[index* 2+ 0]= mesh->skin.vertex_bone_indices[position_index* 2+ 0];
		skin.vertex_bone_indices[index* 2+ 1]= mesh->skin.vertex_bone_indices[position_index* 2+ 1];
		skin.vertex_weights[index* 2+ 0]= mesh->skin.vertex_weights[position_index* 2+ 0];
		skin.vertex_weights[index* 2+ 1]= mesh->skin.vertex_weights[position_index* 2+ 1];
	}

	mesh->positions= positions;
	mesh->texture_coordinates= texture_coordinates;
	mesh->normals= normals;

	mesh->skin.vertex_bone_indices= skin.vertex_bone_indices;
	mesh->skin.vertex_weights= skin.vertex_weights;
}

string Mesh::MakeFilepath(string filename)
{
	return "data/assets/graphical/models/"+ filename;
}

vector<Mesh *> Mesh::Parse(string filename)
{
	string filepath= MakeFilepath(filename);

	vector<Mesh *> meshes;

	vector<string> material_names;
	vector<Material *> materials;

	std::map<string, std::map<int, float>> skinning_data;

	int position_index_offset= 1;
	int normal_index_offset= 1;
	int texture_coordinate_index_offset= 1;


	Mesh *mesh= nullptr;

	std::ifstream input_stream;
	input_stream.open(filepath);

	string material_library_filename;
	string skeleton_library_filename;

	string skin_name;
	string skeleton_name;

	bool file_already_parsed= false;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = Utility::TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if(tokens[0] == "o")
		{
			if(mesh!= nullptr)
			{
				if(!file_already_parsed)
				{
					position_index_offset+= (int)(mesh->positions.size()/ 3);
					normal_index_offset+= (int)(mesh->normals.size()/ 3);
					texture_coordinate_index_offset+= (int)(mesh->texture_coordinates.size()/ 2);

					BakeSkin(mesh, skinning_data);
					EnforceUniformIndexOrder(mesh);

					skinning_data.clear();
				}

				meshes.push_back(mesh);
			}

			if(System::resource.GetResource(MakeName(filepath, tokens[1]))!= nullptr)
			{
				mesh= Retrieve(filename, tokens[1]);
				file_already_parsed= true;
			}
			else
				mesh= new Mesh(filepath, tokens[1]);
		}
		else if(!file_already_parsed)
		{
			if (tokens[0] == "mtllib")
				materials= Material::Parse(tokens[1]);

			else if(tokens[0] == "v")
			{
				mesh->positions.push_back((float)atof(tokens[1].c_str()));
				mesh->positions.push_back((float)atof(tokens[2].c_str()));
				mesh->positions.push_back((float)atof(tokens[3].c_str()));
			}

			else if(tokens[0] == "vt")
			{
				float u= (float)atof(tokens[1].c_str());
				if(abs(u)>= 1)
					u-= (int)u;
				if(u< 0)
					u+= 1;

				float v= 1- (float)atof(tokens[2].c_str());
				if(abs(v)>= 1)
					v-= (int)v;
				if(v< 0)
					v+= 1;

				mesh->texture_coordinates.push_back(u);
				mesh->texture_coordinates.push_back(v);
			}

			else if(tokens[0] == "vn")
			{
				mesh->normals.push_back((float)atof(tokens[1].c_str()));
				mesh->normals.push_back((float)atof(tokens[2].c_str()));
				mesh->normals.push_back((float)atof(tokens[3].c_str()));
			}

			else if(tokens[0] == "usemtl")
				material_names.push_back(tokens[1]);

			else if(tokens[0] == "f")
			{
				for(unsigned int i = 1; i < tokens.size(); i++)
				{
					std::replace(tokens[i].begin(), tokens[i].end(), '/', ' ');
					vector<string> token_tokens = Utility::TokenizeOverSpaces(tokens[i]);

					int token_index= 0;

					mesh->position_indices.push_back(atoi(token_tokens[token_index++].c_str())- position_index_offset);
					if(mesh->texture_coordinates.size()> 0) mesh->texture_coordinate_indices.push_back(atoi(token_tokens[token_index++].c_str())- texture_coordinate_index_offset);
					if(mesh->normals.size()> 0) mesh->normal_indices.push_back(atoi(token_tokens[token_index++].c_str())- normal_index_offset);

				}

				//quads
				if(tokens.size()== 5)
				{
					int last= (int)(mesh->position_indices.size()- 1);

					mesh->position_indices.push_back(mesh->position_indices[last- 3]);
					if(mesh->texture_coordinates.size()> 0) mesh->texture_coordinate_indices.push_back(mesh->texture_coordinate_indices[last- 3]);
					if(mesh->normals.size()> 0) mesh->normal_indices.push_back(mesh->normal_indices[last- 3]);

					mesh->position_indices.push_back(mesh->position_indices[last- 1]);
					if(mesh->texture_coordinates.size()> 0) mesh->texture_coordinate_indices.push_back(mesh->texture_coordinate_indices[last- 1]);
					if(mesh->normals.size()> 0) mesh->normal_indices.push_back(mesh->normal_indices[last- 1]);
				}
			}

			else if(tokens[0] == "skin")
				skin_name= tokens[1];

			else if(tokens[0] == "weight")
				skinning_data[skin_name][atoi(tokens[1].c_str())]= (float)atof(tokens[2].c_str());

			else if(tokens[0] == "skeletonlib")
				skeleton_library_filename= tokens[1];

			else if(tokens[0] == "useskeleton")
				skeleton_name= tokens[1];
		}
	}
	input_stream.close();
	if(mesh!= nullptr)
	{
		if(!file_already_parsed)
		{
			BakeSkin(mesh, skinning_data);
			EnforceUniformIndexOrder(mesh);
		}

		meshes.push_back(mesh);
	}

	if(!file_already_parsed)
	{
		for(unsigned int i= 0; i< meshes.size(); i++)
		{
			Material *material= nullptr;
			for(unsigned int j= 0; j< materials.size(); j++)
				if(materials[j]->GetElementPath()[0]== material_names[i])
				{
					material= materials[j];
					break;
				}
			if(material== nullptr)
				material= Material::Retrieve("default.mtl", "Material");

			meshes[i]->material= material;
		}

		if(skeleton_library_filename!= "")
			mesh->skeleton= Skeleton::Retrieve(skeleton_library_filename, skeleton_name);
	}

	return meshes;
}

Mesh::Mesh(string filepath, string element_name)
	: FileResource(filepath, element_name)
{
	Clear();
}