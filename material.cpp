#include "Material.h"
#include "System.h"

#include <fstream>

vector<Material *> Material::materials_;

vector<Material *> Material::GetMaterials()
{
	return materials_;
}

string Material::MakeFilepath(string filename)
{
	return "data/assets/graphical/materials/"+ filename;
}

vector<Material *> Material::Parse(string filename)
{
	string filepath= MakeFilepath(filename);

	vector<Material *> materials;

	std::ifstream input_stream(filepath);

	PhongMaterial *material= nullptr;

	while(input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = Utility::TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if(tokens[0] == "newmtl")
		{
			if(material!= nullptr)
				materials.push_back(material);

			material= new PhongMaterial(filepath, tokens[1]);
			
			material->reflectivity= 0.0f;
			material->transparency= 0.0f;
		}
		else if(tokens[0] == "Kd")
		{
			material->diffuse= Color((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
			//material->diffuse= Color((float)atof(tokens[3].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[1].c_str()));
		}
		else if(tokens[0] == "Ks")
			material->specular= Color((float)atof(tokens[1].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[3].c_str()));
			//material->specular= Color((float)atof(tokens[3].c_str()), (float)atof(tokens[2].c_str()), (float)atof(tokens[1].c_str()));
		else if(tokens[0] == "Ns")
			material->glossiness= (float)atof(tokens[1].c_str());
		else if(tokens[0] == "Ni")
			material->refractive_index= (float)atof(tokens[1].c_str());
		else if(tokens[0] == "Tr")
			material->transparency= (float)atof(tokens[1].c_str());
		else if(tokens[0] == "Rf")
			material->reflectivity= (float)atof(tokens[1].c_str());

		else if(tokens[0] == "map_Kd")
			material->diffuse_texture= ImageFile::Retrieve(Utility::GetFilename(tokens[tokens.size()- 1]));
	}
	input_stream.close();
	if(material!= nullptr)
		materials.push_back(material);


	return materials;
}

Material::Material(string filepath, string element_name)
	: FileResource(filepath, element_name)
{
	materials_.push_back(this);
}

PhongMaterial::PhongMaterial(string filepath, string element_name)
	: Material(filepath, element_name)
{
	
}
	