#include "Data.h"
#include "Math.h"

#include <fstream>


ISPCData::ISPCData(int max_count)
{
	shading.count= 0;
	shading.max_count= max_count;
	shading.collect_data= true;

	shading.normals= new float[shading.max_count* 3];
	shading.directions= new float[shading.max_count* 3];
	shading.image_xys= new float[shading.max_count* 2];
	shading.bounce_counts= new int[shading.max_count* 1];
	shading.incoming_ray_types= new int[shading.max_count* 1];
	shading.outgoing_ray_types= new int[shading.max_count* 1];

	shading.indicies= new int[shading.max_count* 1];
}

#define HISTOGRAM_SIZE 10
void ISPCData::Write()
{
#if 0
	float histogram[HISTOGRAM_SIZE];
	memset(histogram, 0, sizeof(float)* HISTOGRAM_SIZE);

			
	for(int i= 0; i< shading.count; i++)
	{
		Vec3f normal= MakeVec3f(shading.normals+ i* 3);
		Vec3f direction= MakeVec3f(shading.directions+ i* 3);
		float cos_theta= dot(normal, direction);

		if(cos_theta> 1 || cos_theta< 0)
		{
			cos_theta= std::max(std::min(cos_theta, 1.0f), 0.0f);
		}

		float y= cos_theta;
		float x= acos(y);
		y/= 2* M_PI* sin(x);

		int index= (int)(HISTOGRAM_SIZE* x/ (M_PI/ 2));
		histogram[index]+= y;
	}

	std::ofstream output("histogram_actual5.csv");
	for(int i= 0; i< HISTOGRAM_SIZE; i++)
	{
		output << (i+ 1)/ 20.0f << ", " << histogram[i] << endl;
	}
	output.close();
#endif

	std::ofstream output2("points.xyz");
	int foo= 0;
	for(int i= 0; i< shading.count; i++)
	{
		Vec3f normal= MakeVec3f(shading.normals+ i* 3);

		int x= shading.image_xys[i* 2+ 0];
		int y= shading.image_xys[i* 2+ 1];
		int bounce_count= shading.bounce_counts[i];

		//if(x>= 0 && x<= 3 && y>= 330 && y<= 333  && bounce_count== 1)
		//if(x>= 1 && x<= 1 && y>= 330 && y<= 330  && bounce_count== 1)
		if(x>= 25 && x<= 26 && y>= 27 && y<= 27  && bounce_count== 2)
		{
			if(foo>= shading.max_count)
				break;
			Vec3f direction= MakeVec3f(shading.directions+ i* 3); 

			shading.normals[foo* 3+ 0]= normal.x;
			shading.normals[foo* 3+ 1]= normal.y;
			shading.normals[foo* 3+ 2]= normal.z;

			shading.directions[foo* 3+ 0]= direction.x;
			shading.directions[foo* 3+ 1]= direction.y;
			shading.directions[foo* 3+ 2]= direction.z;

			shading.image_xys[foo* 2+ 0]= x;
			shading.image_xys[foo* 2+ 1]= y;

			shading.bounce_counts[foo]= bounce_count;

			foo++;

			float component= dot(normal, direction);
			direction= direction- (normal* component);

			output2 << (direction.x+ 2)+ (normal.x* 10)+ (x== 26 ? 30 : 0) << " " << direction.y+ (normal.y* 10) << " " << direction.z+ (normal.z* 10) << endl;
		}
	}
	output2.close();

	shading.count= foo;
}