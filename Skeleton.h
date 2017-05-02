#ifndef PS_SKELETON
#define PS_SKELETON

#include "Vector.h"
#include "Transform.h"
#include "FileResource.h"
#include "Common.h"

#include <map>


class Skeleton : public FileResource<Skeleton>
{
public:
	struct Bone
	{
		string name;
		Vec3f tail, head;
		Vec3f forward, up, right;

		string parent_name;

		Bone(string name, Vec3f tail, Vec3f head, string parent_name= "");
		Bone();
	};

private:
	struct Action : public FileResource<Action>
	{
		struct Key
		{
			string bone_name;

			Vec3f position;
			Vec3f scale;
			float pitch, yaw, roll;

			Key(string bone_name, Vec3f position, Vec3f scale, float pitch, float yaw, float roll);
			Key();
		};

		struct Frame
		{
			std::map<string, Key> keys;
		};
	
		string name;
		std::map<int, Frame> frames;

		Action(string filepath, string element_name);
		Action();

		int last_frame_number= -1;
		int GetLastFrameNumber();

		Key Skeleton::Action::GetKey(float frame_value, string bone_name);

		static string MakeFilepath(string filename);
		static vector<Action *> Parse(string filename);
	};

	std::map<string, Bone> bones;
	std::map<string, Action *> actions;

	Skeleton(string filepath, string element_name);

	void GetBoneTransform(string bone_name, string action_name, float frame_value, Transform &rotation, Vec3f &displacement);

public:
	Bone GetBone(string bone_name);
	Transform GetBoneTransform(string bone_name, string action_name, float moment);

	static string MakeFilepath(string filename);
	static vector<Skeleton *> Parse(string filename);
};

#endif
