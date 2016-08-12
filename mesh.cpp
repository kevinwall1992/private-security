#include "Mesh.h"

#include <fstream>
#include <algorithm>


Mesh::Mesh(string filename)
{
	LoadOBJ(filename);
}

Mesh::Mesh()
{

}

int Mesh::GetTriangleCount()
{
	return (int)(position_indices.size()/ 3);
}

int Mesh::GetVertexCount()
{
	return (int)(positions.size()/ 3);
}

void Mesh::LoadOBJ(string filename)
{
	Clear();

	std::ifstream input_stream;
	input_stream.open(filename);

	string material_library_filename;
	string current_material_name;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if (tokens[0] == "mtllib")
			material_library_filename = tokens[1];

		else if (tokens[0] == "v")
		{
			positions.push_back((float)atof(tokens[1].c_str()));
			positions.push_back((float)atof(tokens[2].c_str()));
			positions.push_back((float)atof(tokens[3].c_str()));
		}

		else if (tokens[0] == "vt")
		{
			texture_coordinates.push_back((float)atof(tokens[1].c_str()));
			texture_coordinates.push_back((float)atof(tokens[2].c_str()));
		}

		else if(tokens[0] == "vn")
		{
			normals.push_back((float)atof(tokens[1].c_str()));
			normals.push_back((float)atof(tokens[2].c_str()));
			normals.push_back((float)atof(tokens[3].c_str()));
		}

		else if (tokens[0] == "usemtl")
			current_material_name= tokens[1];

		else if (tokens[0] == "f")
		{
			for (unsigned int i = 1; i < tokens.size(); i++)
			{
				std::replace(tokens[i].begin(), tokens[i].end(), '/', ' ');
				vector<string> token_tokens = TokenizeOverSpaces(tokens[i]);

				int token_index= 0;

				position_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);
				if(texture_coordinates.size()> 0) texture_coordinate_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);
				if(normals.size()> 0) normal_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);

			}

			//quads
			if(tokens.size()== 5)
			{
				int last= (int)(position_indices.size()- 1);

				position_indices.push_back(position_indices[last- 3]);
				if(texture_coordinates.size()> 0) texture_coordinate_indices.push_back(texture_coordinate_indices[last- 3]);
				if(normals.size()> 0) normal_indices.push_back(normal_indices[last- 3]);

				position_indices.push_back(position_indices[last- 1]);
				if(texture_coordinates.size()> 0) texture_coordinate_indices.push_back(texture_coordinate_indices[last- 1]);
				if(normals.size()> 0) normal_indices.push_back(normal_indices[last- 1]);
			}
		}
	}
	input_stream.close();
}

void Mesh::Clear()
{
	positions.clear();
	texture_coordinates.clear();
	normals.clear();

	position_indices.clear();
	texture_coordinate_indices.clear();
	normal_indices.clear();
}