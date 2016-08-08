#include <embree2\rtcore.h>
#include <embree2\rtcore_ray.h>
#include "embree_util\math\vec3.h"

using embree::Vec3f;
using embree::normalize;
using embree::cross;
using embree::dot;
typedef Vec3f Color;

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>
#include <condition_variable>
#include <xmmintrin.h>
#include <pmmintrin.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::vector;
using std::queue;

#include <Windows.h>

#include <GL\glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "Parameters.h"


struct Vertex
{
	float x, y, z, w;

	Vertex(float x_, float y_, float z_)
	{
		x= x_; y= y_; z= z_;
	}

	Vertex()
	{
		x= 0.0f; y= 0.0f; z= 0.0f;
	}
};

struct Triangle
{
	int a, b, c;

	Triangle(int a_, int b_, int c_)
	{
		a= a_; b= b_; c= c_;
	}

	Triangle()
	{
		a= -1; b= -1; c= -1;
	}
};

void ErrorHandler(const RTCError code, const char* str)
{
	if(code== RTC_NO_ERROR)
		return;

	cout << "Error: " << code << "; " << str << endl;
}

GLuint CreateShader(GLenum shader_type, const char *shader_source, const char *name= nullptr)
{
	GLuint shader= glCreateShader(shader_type);

	glShaderSource(shader, 1, &shader_source, nullptr);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

		GLchar *info_log = new GLchar[info_log_length + 1];
		glGetShaderInfoLog(shader, info_log_length, NULL, info_log);

		cout << "Compilation failure in " << (name== nullptr ? "shader" : name) << ". " << info_log;
		delete[] info_log;
	}

	return shader;
}

GLuint CreateShaderProgram(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint shader_program= glCreateProgram();

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	glLinkProgram(shader_program);

	GLint status;
	glGetProgramiv (shader_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint info_log_length;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_log_length);

		GLchar *info_log = new GLchar[info_log_length + 1];
		glGetProgramInfoLog(shader_program, info_log_length, NULL, info_log);
		cout << "Linking failure: " << info_log;
		delete[] info_log;
	}

	glDetachShader(shader_program, vertex_shader);
	glDeleteShader(vertex_shader);
	glDetachShader(shader_program, fragment_shader);
	glDeleteShader(fragment_shader);

	return shader_program;
}

struct Mesh
{
	vector<float> positions;
	vector<float> texture_coordinates;
	vector<float> normals;

	vector<int> position_indices;
	vector<int> texture_coordinate_indices;
	vector<int> normal_indices;

	int TriangleCount()
	{
		return position_indices.size()/ 3;
	}

	int VertexCount()
	{
		return positions.size()/ 3;
	}
};

//http://stackoverflow.com/questions/236129/splitting-a-string-in-c
vector<string> TokenizeOverSpaces(string string_)
{
	std::istringstream input_string_stream(string_);

	vector<string> tokens;
	copy(std::istream_iterator<string>(input_string_stream),
			std::istream_iterator<string>(),
			std::back_inserter<vector<string> >(tokens));

	return tokens;
}

Mesh LoadOBJ(string filename)
{
	Mesh mesh;

	ifstream input_stream;
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
			mesh.positions.push_back((float)atof(tokens[1].c_str()));
			mesh.positions.push_back((float)atof(tokens[2].c_str()));
			mesh.positions.push_back((float)atof(tokens[3].c_str()));
		}

		else if (tokens[0] == "vt")
		{
			mesh.texture_coordinates.push_back((float)atof(tokens[1].c_str()));
			mesh.texture_coordinates.push_back((float)atof(tokens[2].c_str()));
		}

		else if(tokens[0] == "vn")
		{
			mesh.normals.push_back((float)atof(tokens[1].c_str()));
			mesh.normals.push_back((float)atof(tokens[2].c_str()));
			mesh.normals.push_back((float)atof(tokens[3].c_str()));
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

				mesh.position_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);
				if(mesh.texture_coordinates.size()> 0) mesh.texture_coordinate_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);
				if(mesh.normals.size()> 0) mesh.normal_indices.push_back(atoi(token_tokens[token_index++].c_str())- 1);

			}

			//quads
			if(tokens.size()== 5)
			{
				int last= (int)(mesh.position_indices.size()- 1);

				mesh.position_indices.push_back(mesh.position_indices[last- 3]);
				if(mesh.texture_coordinates.size()> 0) mesh.texture_coordinate_indices.push_back(mesh.texture_coordinate_indices[last- 3]);
				if(mesh.normals.size()> 0) mesh.normal_indices.push_back(mesh.normal_indices[last- 3]);

				mesh.position_indices.push_back(mesh.position_indices[last- 1]);
				if(mesh.texture_coordinates.size()> 0) mesh.texture_coordinate_indices.push_back(mesh.texture_coordinate_indices[last- 1]);
				if(mesh.normals.size()> 0) mesh.normal_indices.push_back(mesh.normal_indices[last- 1]);
			}
		}
	}
	input_stream.close();

	return mesh;
}


class EmbreeSystem;
class GraphicsSystem;
class System
{
	static bool systems_initialized;

protected:
	virtual void Initialize()= 0;
	virtual void Terminate()= 0;

public:
	static EmbreeSystem embree;
	static GraphicsSystem graphics;
	static void InitializeSystems();
	static void TerminateSystems();
};


class EmbreeSystem : public System
{
protected:
	void Initialize();
	void Terminate();

public:
	RTCDevice device;

	friend System;
};


class GraphicsSystem : public System
{
	
protected:
	void Initialize();
	void Terminate();

public:
	//May want to make these private
	SDL_Window *main_window;
	SDL_Renderer *renderer;
	SDL_GLContext opengl_context;
	int screen_width= 512;
	int screen_height= 512;

	friend System;
};


#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y
#define RTCRayPacket JOIN(RTCRay, PACKET_SIZE)//need to test compile time packet sizes vs run time packet sizes


struct RayType{enum Enum {Primary, Reflection, Refraction, Indirect};};

struct RayExtras//Change name probably
{
	float x, y;
	Color absorption;
	int bounce_count;
	RayType::Enum type;

	RayExtras(float x, float y, Color absorption, int bounce_count, RayType::Enum type);
	RayExtras();
};


//Doesn't work if you don't have a source for the pointers to point to that won't be deleted when scope changes
//Would otherwise have to dynamically allocate the pointers, which is wonky and a waste of memory since they
//eventually get stored in already allocated memory

//Look at rethink here
struct CompleteRay
{
	RTCRay *ray;
	RayExtras *extras;

	CompleteRay(RTCRay *ray, RayExtras *extras);
};

struct CompleteRayPacket
{
	RTCRayPacket *ray_packet;
	int ray_index;

	RayExtras *extras;

	CompleteRayPacket(RTCRayPacket *ray_packet, int ray_index, RayExtras *ray_extras);
};


class Light
{
public:
	Light();
	
	virtual bool IsAmbient();
	virtual bool IsSoft();
	virtual bool IsGenerative();

	virtual Color GetLuminosity(Vec3f point)= 0;
	virtual Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
	
	//virtual RTCRay SampleLightRay(int sample_index);//put this in GenerativeLight?
};

class AmbientLight : public Light
{
	Color intensity;

public:
	AmbientLight(Color intensity);

	bool IsAmbient();
	Color GetLuminosity(Vec3f point);
}; 

class PointLight : public Light
{
protected:
	Vec3f position;
	Color intensity;

public:
	PointLight(Vec3f position, Color intensity);

	bool IsGenerative();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};

class DiscLight : public PointLight
{
	float radius;

public:
	DiscLight(Vec3f position, Color intensity, float radius);

	bool IsSoft();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index);
};


class Scene
{
	RTCScene embree_scene;
	vector<unsigned int> geometry_ids;
	vector<Light *> lights;
	vector<AmbientLight *> ambient_lights;

public:
	Scene(RTCDevice device);

	void AddOBJ(string filename);
	void AddLight(Light *light);

	vector<Light *> * GetLights();
	vector<AmbientLight *> * GetAmbientLights();

	void Commit();

	RTCScene GetEmbreeScene();//Would like to internalize whatever is calling this
};


struct BlockState{enum Enum {Empty, Partial, Full};};

struct RayBlock
{
	RTCRay rays[RAY_BLOCK_SIZE];
	RayExtras ray_extrass[RAY_BLOCK_SIZE];
	int front_index;

	BlockState::Enum state;
	bool is_primary;
	bool is_coherent;
	//vector<float> occlusion;
	

	RayBlock(bool is_primary, bool is_coherent);
	RayBlock();

	void Empty();
};

/*#define RAY_PACKET_BLOCK_SIZE (128/ PACKET_SIZE)
struct RayPacketBlock
{
	RTCRayPacket ray_packets[RAY_BLOCK_SIZE];
	RayExtras ray_extrass[RAY_BLOCK_SIZE];
	int front_index;
	//vector<float> occlusion;

	RayPacketBlock();
};*/

/*#define SHADOW_BLOCKS_PER_RAY_BLOCK 1
#define SHADOW_BLOCK_SIZE (RAY_BLOCK_SIZE/ SHADOW_BLOCKS_PER_RAY_BLOCK)
struct ShadowBlock
{
	RTCRay *hits;
	RayBlock *ray_block;
};*/


struct Camera;
struct Film;

struct Surface
{
	Vec3f position;
	Vec3f normal;

	Surface(Vec3f position, Vec3f normal);
	Surface();
};

void ShadingKernel(CompleteRay ray, /*Surface &surface, float *occlusions,*/ Film *film, Scene *scene);


//http://stackoverflow.com/questions/24465533/implementing-boostbarrier-in-c11
class Barrier
{
private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::size_t _count;

	std::size_t _original_count;

public:
	explicit Barrier(std::size_t count= THREAD_COUNT) : _count{count}, _original_count{count} { }
    void Wait()
    {
        std::unique_lock<std::mutex> lock{_mutex};
        if (--_count == 0) {
            _cv.notify_all();
        } else {
            _cv.wait(lock, [this] { return _count == 0; });
        }
    }

	void Reset()
	{
		_count= _original_count;
	}
};


//Lets change it back to Job!!!
struct TaskType{enum Enum {None, Refill, Shade, Develop};};

struct Task
{
	TaskType::Enum type;

	union
	{
		struct
		{
			RayBlock *primary_ray_block;
		}refill;

		struct
		{
			RayBlock *ray_block;
		}shade;
	};

	Task(TaskType::Enum type, RayBlock *ray_block= nullptr);
	Task();
};

//Thinking of redesigning this to be a servant of camera.
//Better yet would be a implementation of a Interface for accelerated workers for
//high level objects like Camera

//All these pointers are dissonant with our usual use of value types and references for objects... 
//But I cant use references on a thread call. 
class Shutter
{
	Camera *camera;

	queue<RayBlock *> empty_primary_ray_blocks; 
	queue<RayBlock *> empty_coherent_ray_blocks; 
	queue<RayBlock *> empty_incoherent_ray_blocks; 
	queue<RayBlock *> full_ray_blocks;

	std::thread threads[THREAD_COUNT];
	
	std::atomic_int next_camera_tile_index;
	std::atomic_int next_film_interval_index;

	bool ray_source_exhausted;
	bool develop_finished;

	std::mutex task_mutex;
	std::mutex resource_mutex;

	Barrier barrier;

protected:
	RayBlock * TakeEmptyPrimaryRayBlock();
	RayBlock * TakeEmptyCoherentRayBlock();
	RayBlock * TakeEmptyIncoherentRayBlock();
	RayBlock * TakeFullRayBlock();
	void ReturnRayBlock(RayBlock *ray_block);

	void Refill(RayBlock *primary_ray_block);
	void Shade(RayBlock *ray_block, Scene *scene, Film *film);
	void Develop(Film *film);

	Task GetTask();
	void TaskLoop(Scene *scene);

	void Reset();

public:
	Shutter(Camera *camera= nullptr);

	void Open(Scene &scene);
};


struct Pixel
{
	unsigned char r, g, b;
};

struct Image
{
	Pixel *pixels;

	Image(int width, int height);
	Image();
	~Image();

	void Resize(int width, int height);
};

//Would be nice if we could use a dedicated Color class, 
//if only for r, g, b components. But I don't want to reimplement
//the math in Vec3f. Maybe we don't need much of that stuff. Plus, we do need
//Some things that aren't present in Vec3f. (such as getting the perceptual brightness, etc)
//Let's try switching after we get things working, so we can quantify any possible performance
//issues. (shouldn't be, but might as well)
struct Film
{
	int width, height;
	Vec3f *receptors;
	Image image;

	Film(int width, int height);
	~Film();

	void Clear();//Does this need to be parallelized?
	void Stimulate(int x, int y, Color light);

	void Develop();
	bool Develop_Parallel(int interval_index);//hmmm
};


//consider having queryable lock when generating rays so
//Threads can pass if object is busy
//Want to evenutally have camera control methods that update
//the up, direction, and right vectors to be perpendicular
//(right now we just just generate these when necessary, and they are not very interactive)
struct Camera
{
	Film *film;

	Vec3f position, forward, up, right;
	float fov;
	bool orthograhpic;

	Vec3f view_plane_u;
	Vec3f view_plane_v;

	Camera(float fov_in_degrees, Vec3f position, Vec3f direction= Vec3f(0, 0, -1));

	void LookAt(Vec3f look_at_position);
	void OrthogonalizeVectors();
	void ComputeViewPlane();

	Film * LoadFilm(Film *film);
	Film * RemoveFilm();

	void GetRays(CompleteRay first_ray, int &count, int tile_index);
	//RayPacketBlock GetRayPackets();

	Image *TakePicture(Scene &scene);

private:
	//Think about making Shutter internal
	Shutter shutter;//Even more reason to rename to something more appropriate

};