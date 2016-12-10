#include "Prop.h"
#include "Common.h"

#include <fstream>
#include <algorithm>

Prop::Prop(Mesh *mesh_, Material *material_)
{
	mesh= mesh_;
	material= material_;
}

Prop::Prop()
{
	mesh= nullptr;
	material= nullptr;
}

vector<Material *> ParseMaterialLibrary(string filename)
{
	vector<Material *> materials;

	std::ifstream input_stream("scenes/"+ filename);

	PhongMaterial *material= nullptr;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if (tokens[0] == "newmtl")
		{
			if(material!= nullptr)
				materials.push_back(material);

			material= new PhongMaterial(tokens[1]);
			
			material->reflectivity= 0.0f;
			material->transparency= 0.0f;
		}
		else if (tokens[0] == "Kd")
			material->diffuse= Color((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
			//material->diffuse= Color((float)atof(tokens[3].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[1].c_str()));
		else if (tokens[0] == "Ks")
			material->specular= Color((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
			//material->specular= Color((float)atof(tokens[3].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[1].c_str()));
		else if (tokens[0] == "Ns")
			material->glossiness= (float)atof(tokens[1].c_str())/ 1000;
		else if (tokens[0] == "Ni")
			material->refractive_index= (float)atof(tokens[1].c_str());
		else if (tokens[0] == "Tr")
			material->transparency= (float)atof(tokens[1].c_str());
		else if (tokens[0] == "Rf")
			material->reflectivity= (float)atof(tokens[1].c_str());
	}
	input_stream.close();
	if(material!= nullptr)
		materials.push_back(material);


	return materials;
}

vector<Prop> Prop::ParseOBJ(string filename)
{
	vector<Mesh *> meshes;
	vector<string> material_names;

	vector<Material *> materials;

	int position_index_offset= 1;
	int normal_index_offset= 1;
	int texture_coordinate_index_offset= 1;


	Mesh *mesh= nullptr;

	std::ifstream input_stream;
	input_stream.open(filename);

	string material_library_filename;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if (tokens[0] == "mtllib")
			materials= ParseMaterialLibrary(tokens[1]);

		else if(tokens[0] == "o")
		{
			if(mesh!= nullptr)
			{
				meshes.push_back(mesh);

				position_index_offset+= mesh->positions.size()/ 3;
				normal_index_offset+= mesh->normals.size()/ 3;
				texture_coordinate_index_offset+= mesh->texture_coordinates.size()/ 3;
			}

			mesh= new Mesh(tokens[1]);
		}

		else if (tokens[0] == "v")
		{
			mesh->positions.push_back((float)atof(tokens[1].c_str()));
			mesh->positions.push_back((float)atof(tokens[2].c_str()));
			mesh->positions.push_back((float)atof(tokens[3].c_str()));
		}

		else if (tokens[0] == "vt")
		{
			mesh->texture_coordinates.push_back((float)atof(tokens[1].c_str()));
			mesh->texture_coordinates.push_back((float)atof(tokens[2].c_str()));
		}

		else if(tokens[0] == "vn")
		{
			mesh->normals.push_back((float)atof(tokens[1].c_str()));
			mesh->normals.push_back((float)atof(tokens[2].c_str()));
			mesh->normals.push_back((float)atof(tokens[3].c_str()));
		}

		else if (tokens[0] == "usemtl")
			material_names.push_back(tokens[1]);

		else if (tokens[0] == "f")
		{
			for (unsigned int i = 1; i < tokens.size(); i++)
			{
				std::replace(tokens[i].begin(), tokens[i].end(), '/', ' ');
				vector<string> token_tokens = TokenizeOverSpaces(tokens[i]);

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
	}
	input_stream.close();
	if(mesh!= nullptr)
		meshes.push_back(mesh);

	vector<Prop> props;
	for(unsigned int i= 0; i< meshes.size(); i++)
	{
		Material *material= nullptr;
		for(unsigned int j= 0; j< materials.size(); j++)
			if(materials[j]->GetName()== material_names[i])
			{
				material= materials[j];
				break;
			}
		if(material== nullptr)
			material= Material::GetDefaultMaterial();

		props.push_back(Prop(meshes[i], material));
	}

	return props;
}