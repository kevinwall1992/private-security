#include "Skeleton.h"

#include <fstream>


Skeleton::Bone::Bone(string name_, Vec3f tail_, Vec3f head_, string parent_name_)
{
	name= name_;
	tail= tail_;
	head= head_;
	parent_name= parent_name_;
}

Skeleton::Bone::Bone()
{
	
}

Skeleton::Action::Key::Key(string bone_name_, Vec3f position_, Vec3f scale_, float pitch_, float yaw_, float roll_)
{
	bone_name= bone_name_;
	position= position_;
	scale= scale_;
	pitch= pitch_;
	yaw= yaw_;
	roll= roll_;
}

Skeleton::Action::Key::Key()
{
}

Skeleton::Action::Action(string filepath, string element_name)
	: FileResource(filepath, element_name)
{
}

Skeleton::Action::Action()
	: FileResource("Error")
{
}

int Skeleton::Action::GetLastFrameNumber()
{
	if(last_frame_number> -1)
		return last_frame_number;

	for(auto pair: frames)
		if(pair.first> last_frame_number)
			last_frame_number= pair.first;

	return last_frame_number;
}

Skeleton::Action::Key Skeleton::Action::GetKey(float frame_value, string bone_name)
{
	frame_value+= 1;

	int frame_number_a= (int)frame_value;
	int frame_number_b= frame_number_a+ 1;

	float a_weight= frame_number_b- frame_value;
	float b_weight= 1- a_weight;

	if(frame_number_a> GetLastFrameNumber())
		frame_number_a= frame_number_a% GetLastFrameNumber();
	if(frame_number_b> GetLastFrameNumber())
		frame_number_b= frame_number_b% GetLastFrameNumber();

	if(!Utility::Contains(frames[frame_number_a].keys, bone_name))
		a_weight= 0;
	if(!Utility::Contains(frames[frame_number_b].keys, bone_name))
		b_weight= 0;

	if(a_weight== 0 && b_weight== 0)
		return Key(bone_name, Vec3f(), Vec3f(1, 1, 1), 0, 0, 0);
	else
	{
		float total_weight= a_weight+ b_weight;
		a_weight= a_weight/ total_weight;
		b_weight= 1- a_weight;
	}

	Key a= frames[frame_number_a].keys[bone_name];
	Key b= frames[frame_number_b].keys[bone_name];

	return Key(a.bone_name, 
		       a.position* a_weight+ b.position* b_weight, 
			   a.scale* a_weight+ b.scale* b_weight, 
			   a.pitch* a_weight+ b.pitch* b_weight, 
			   a.yaw* a_weight+ b.yaw* b_weight, 
			   a.roll* a_weight+ b.roll* b_weight);
}

string Skeleton::Action::MakeFilepath(string filename)
{
	return "data/assets/graphical/skeletons/"+ filename;
}

vector<Skeleton::Action *> Skeleton::Action::Parse(string filename)
{
	vector<Action *> actions;
	Action *action= nullptr;

	string filepath= MakeFilepath(filename);

	std::ifstream input_stream;
	input_stream.open(filepath);

	int frame_number;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = Utility::TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if (tokens[0] == "action")
		{
			if(action!= nullptr)
				actions.push_back(action);

			action= new Action(filepath, tokens[1]);
			action->name= tokens[1];
		}

		else if(tokens[0] == "frame")
			frame_number= atoi(tokens[1].c_str());

		else if(tokens[0] == "key")
		{
			string bone_name= tokens[1];
			action->frames[frame_number].keys[bone_name].bone_name= bone_name;

			if(tokens[2]== "position")
				action->frames[frame_number].keys[bone_name].position= Vec3f((float)atof(tokens[3].c_str()), 
																				 (float)atof(tokens[4].c_str()), 
																				 (float)atof(tokens[5].c_str()));
			else if(tokens[2]== "rotation")
			{
				action->frames[frame_number].keys[bone_name].pitch= (float)atof(tokens[3].c_str());
				action->frames[frame_number].keys[bone_name].yaw= (float)atof(tokens[4].c_str());
				action->frames[frame_number].keys[bone_name].roll= (float)atof(tokens[5].c_str());
			}
			else if(tokens[2]== "scale")
				action->frames[frame_number].keys[bone_name].scale= Vec3f((float)atof(tokens[3].c_str()), 
																		  (float)atof(tokens[4].c_str()), 
																		  (float)atof(tokens[5].c_str()));
		}
	}
	input_stream.close();
	if(action!= nullptr)
		actions.push_back(action);

	return actions;
}

Skeleton::Skeleton(string filepath, string element_name)
	: FileResource(filepath, element_name)
{
	vector<Action *> actions= Action::Parse(Utility::GetFilename(filepath));

	for(Action *action : actions)
		this->actions[action->name]= action;
}

void Skeleton::GetBoneTransform(string bone_name, string action_name, float frame_value, Transform &rotation, Vec3f &displacement)
{
	Bone bone= bones[bone_name];
	Action::Key key= actions[action_name]->GetKey(frame_value, bone_name);
	Transform local_rotation= Transform().RotatedAboutAxis(key.roll, bone.forward).RotatedAboutAxis(key.pitch, bone.right).RotatedAboutAxis(key.yaw, bone.up).Scale(key.scale);
	Vec3f local_displacement= bone.forward* key.position.y+ bone.up* key.position.z+ bone.right* key.position.x;
	
	if(bone.parent_name!= "")
	{
		GetBoneTransform(bone.parent_name, action_name, frame_value, rotation, displacement);
		displacement+= rotation.Apply((bone.head- bones[bone.parent_name].head)+ local_displacement);
		rotation= local_rotation.Merge(rotation);
	}
	else
	{
		displacement= local_displacement+ bone.head;
		rotation= local_rotation;
	}
}

Skeleton::Bone Skeleton::GetBone(string bone_name)
{
	return bones[bone_name];
}

Transform Skeleton::GetBoneTransform(string bone_name, string action_name, float moment)
{
	Transform rotation;
	Vec3f displacement;
	GetBoneTransform(bone_name, action_name, moment* actions[action_name]->GetLastFrameNumber(), rotation, displacement);

	return rotation.Translate(displacement);
}

string Skeleton::MakeFilepath(string filename)
{
	return "data/assets/graphical/skeletons/"+ filename;
}

vector<Skeleton*> Skeleton::Parse(string filename)
{
	vector<Skeleton *> skeletons;
	Skeleton *skeleton= nullptr;

	string filepath= MakeFilepath(filename);

	std::ifstream input_stream;
	input_stream.open(filepath);

	string bone_name;

	while (input_stream.good())
	{
		string line;
		getline(input_stream, line);
		vector<string> tokens = Utility::TokenizeOverSpaces(line);

		if(tokens.size()== 0)
			continue;

		if (tokens[0] == "skeleton")
		{
			if(skeleton!= nullptr)
				skeletons.push_back(skeleton);

			skeleton= new Skeleton(filepath, tokens[1]);
		}

		else if(tokens[0] == "bone")
		{
			bone_name= tokens[1];
			skeleton->bones[bone_name].name= bone_name;
		}

		else if(tokens[0] == "tail")
			skeleton->bones[bone_name].tail= Vec3f((float)atof(tokens[1].c_str()), 
				                                   (float)atof(tokens[2].c_str()), 
				                                   (float)atof(tokens[3].c_str()));

		else if(tokens[0] == "head")
			skeleton->bones[bone_name].head= Vec3f((float)atof(tokens[1].c_str()), 
				                                   (float)atof(tokens[2].c_str()), 
				                                   (float)atof(tokens[3].c_str()));

		else if(tokens[0] == "parent")
			skeleton->bones[bone_name].parent_name= tokens[1];

		else if(tokens[0] == "forward")
			skeleton->bones[bone_name].forward= Vec3f((float)atof(tokens[1].c_str()), 
				                                   (float)atof(tokens[2].c_str()), 
				                                   (float)atof(tokens[3].c_str()));

		else if(tokens[0] == "up")
			skeleton->bones[bone_name].up= Vec3f((float)atof(tokens[1].c_str()), 
				                                   (float)atof(tokens[2].c_str()), 
				                                   (float)atof(tokens[3].c_str()));

		else if(tokens[0] == "right")
			skeleton->bones[bone_name].right= Vec3f((float)atof(tokens[1].c_str()), 
				                                   (float)atof(tokens[2].c_str()), 
				                                   (float)atof(tokens[3].c_str()));
	}
	input_stream.close();
	if(skeleton!= nullptr)
		skeletons.push_back(skeleton);

	return skeletons;
}
