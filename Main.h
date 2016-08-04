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
		x= x_;
		y= y_;
		z= z_;
	}

	Vertex()
	{
		x= 0.0f;
		y= 0.0f;
		z= 0.0f;
	}
};

struct Triangle
{
	int a, b, c;

	Triangle(int a_, int b_, int c_)
	{
		a= a_;
		b= b_;
		c= c_;
	}

	Triangle()
	{
		a= -1;
		b= -1;
		c= -1;
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
			//mesh.positions.push_back(0.0f);//Deciding not to pollute data source
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

//TODO: Change Vertex and Triangle names (preferably replace with something more general)
RTCDevice device;
//RTCScene scene;
void InitializeEmbree()
{
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	device = rtcNewDevice(nullptr);
	ErrorHandler(rtcDeviceGetError(device), "creation");
	rtcDeviceSetErrorFunction(device,ErrorHandler);
}

void ConcludeEmbree()
{
	rtcDeleteDevice(device);
}


const char *vertex_shader_source= 
	"#version 150\n"
	"in vec3 position;\n"
	"out vec2 texture_coordinates;"
	"void main() { texture_coordinates= (position.xy+ vec2(1.0, 1.0))/ 2.0; gl_Position= vec4(position.xy, 0.0, 1.0); }\n";

const char *fragment_shader_source= 
	"#version 150\n"
	"in vec2 texture_coordinates;\n"
	"uniform sampler2D image;\n"
	"void main() { gl_FragColor= texture(image, texture_coordinates.xy); }\n";

const int quad_vertex_count= 3* 6;
float quad_vertices[quad_vertex_count]= { -1.0f, -1.0f, +0.0f, +1.0f, -1.0f, +0.0f, -1.0f, +1.0f, +0.0f, 
					                      +1.0f, -1.0f, +0.0f, +1.0f, +1.0f, +0.0f, -1.0f, +1.0f, +0.0f };

SDL_Window *main_window;
SDL_Renderer *renderer;
SDL_GLContext opengl_context;
int screen_width= 512;
int screen_height= 512;
void InitializeOpenGL()
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	main_window = SDL_CreateWindow("8-Bit RayTracer", 
									SDL_WINDOWPOS_CENTERED, 
									SDL_WINDOWPOS_CENTERED, 
									screen_width, 
									screen_height, 
									SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(main_window, -1, 0);
	opengl_context = SDL_GL_CreateContext(main_window);
	SDL_GL_SetSwapInterval(0);
	int glew_result= glewInit();
	if(glew_result!= 0)
		cout << "GlewInit() error, returned " << glew_result;
	else
		cout << "OpenGL context: " << glGetString(GL_VERSION);
	cout << "\n\n";
	glDisable(GL_DEPTH_TEST);

	
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, quad_vertex_count* sizeof(float), quad_vertices, GL_STATIC_DRAW);

	GLuint vertex_shader= CreateShader(GL_VERTEX_SHADER, vertex_shader_source, "vertex shader");
	GLuint fragment_shader= CreateShader(GL_FRAGMENT_SHADER, fragment_shader_source, "fragment shader");
	GLuint shader_program= CreateShaderProgram(vertex_shader, fragment_shader);
	glUseProgram(shader_program);

	GLint position_attribute= glGetAttribLocation(shader_program, "position");
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(position_attribute);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void ConcludeOpenGL()
{

}

#define JOIN(x, y) JOIN_AGAIN(x, y)
#define JOIN_AGAIN(x, y) x ## y
#define RTCRayPacket JOIN(RTCRay, PACKET_SIZE)//need to test compile time packet sizes vs run time packet sizes


struct RayType{enum Enum {Primary, Reflection, Refraction, Indirect};};

struct RayAncillaries//Change name probably
{
	float x, y;
	Color absorption;
	int bounce_count;
	RayType::Enum type;

	RayAncillaries(float x, float y, Color absorption, int bounce_count, RayType::Enum type);
	RayAncillaries();
};

//Doesn't work if you don't have a source for the pointers to point to that won't be deleted when scope changes
//Would otherwise have to dynamically allocate the pointers, which is wonky and a waste of memory since they
//eventually get stored in already allocated memory
struct CompleteRay
{
	RTCRay *ray;
	RayAncillaries *ray_ancillaries;

	CompleteRay(RTCRay *ray, RayAncillaries *ray_ancillaries);
};

struct CompleteRayPacket
{
	RTCRayPacket *ray_packet;
	int ray_index;

	RayAncillaries *ray_ancillaries;

	CompleteRayPacket(RTCRayPacket *ray_packet, int ray_index, RayAncillaries *ray_ancillaries);
};


class Light
{
public:
	Light();
	
	//Not sure about these functions...
	//Seems redundant to have child defined queries for things that we already
	//Have a class hierarchy for.
	virtual bool IsAmbient() const;//Thinking of removing this
	virtual bool IsSoft();
	virtual bool IsGenerative();

	virtual Color GetLuminosity(Vec3f point) const = 0;
	
	//virtual RTCRay SampleLightRay(int sample_index);//put this in GenerativeLight?
};

class AmbientLight : public Light
{
	Color intensity;

public:
	AmbientLight(Color intensity);

	bool IsAmbient() const;
	Color GetLuminosity(Vec3f point) const;
}; 

class ShadowLight : public Light//Might be a good enough name, but review this
{
public:
	bool IsAmbient() const;

	//Not sure I like the word "Point". Don't use it anywhere else.
	//Silently folding in the distance seems to clever too me, but I'm not doing
	//It to be clever. Its just faster than normalizing only to unormalize, and 
	//cleaner than coming up with some bizzarre name of the function and a way to
	//return the distance separately. So I'd like to review this.
	virtual Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index) const;
};

class PointLight : public ShadowLight
{
protected:
	Vec3f position;
	Color intensity;

public:
	PointLight(Vec3f position, Color intensity);

	bool IsGenerative();

	Color GetLuminosity(Vec3f point) const;
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index) const;
};

class DiscLight : public PointLight
{
	float radius;

public:
	DiscLight(Vec3f position, Color intensity, float radius);

	bool IsSoft();

	Color GetLuminosity(Vec3f point);
	Vec3f SampleDirectionAtPoint(Vec3f point, int sample_index) const;
};


class Scene
{
	RTCScene embree_scene;
	vector<unsigned int> geometry_ids;
	vector<ShadowLight *> shadow_lights;
	vector<AmbientLight *> ambient_lights;

public:
	Scene();

	void AddOBJ(string filename);
	void AddLight(Light *light);

	vector<ShadowLight *> * GetShadowLights();
	vector<AmbientLight *> * GetAmbientLights();

	void Commit();

	RTCScene GetEmbreeScene();
};


struct Pixel
{
	unsigned char r, g, b;
};

struct Image
{
	Pixel *pixels;

	Image(int width, int height);//Seems like this is a little redundant since film exists (or other way around)... Would like to have Film construct Image or something
	~Image();//This may not be a good idea if we aren't dynamically allocating
};

//Would be nice if we could use a dedicated Color class, 
//if only for r, g, b components. But I don't want to reimplement
//the math in Vec3f. Maybe we don't need much of that stuff. Plus, we do need
//Some things that aren't present in Vec3f. (such as getting the perceptual brightness, etc)
//Let's try switching after we get things working, so we can quantify any possible performance
//issues. (shouldn't be, but might as well)
//We should have Stimulate() automatically calculate entropy
//Should we be worried about extra memory needed to store these entropy values?
//Normally, the value is calculated on the fly because the pixels are shaded one at a time
struct Film
{
	int width, height;
	Vec3f *receptors;//try with flat float array

	float max_component;
	std::mutex component_mutex;
	std::atomic_int next_tile_index;

	Film(int width, int height);
	~Film();//This may not be a good idea if we aren't dynamically allocating

	void Clear();
	void Stimulate(int x, int y, Color light);//Name is (still) pretty bad

	//Don't like how this works... Returning image feels better. Could have Film store its own image.
	//Need to parallelize this
	void Develop(Image *image);

	void ComputeMaxComponent();
	bool DevelopJob(Image *image);
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

	std::atomic_int next_tile_index;

	Camera(float fov_in_degrees, Vec3f position, Vec3f direction= Vec3f(0, 0, -1));

	void LookAt(Vec3f look_at_position);
	void OrthogonalizeVectors();
	void ComputeViewPlane();

	Film * LoadFilm(Film *film);
	Film * RemoveFilm();

	//Not sure about this... the responsiblity of the camera is to provide rays, but
	//Keeping track of the progress of the work pool seems wrong. It shouldn't work in 
	//concert and we shouldn't have to interact with its internal operation like this.
	//Also would like more flavorful name for this function
	void Reset();

	void GetRays(CompleteRay first_ray, int &count);
	//RayPacketBlock GetRayPackets();

private:

};


//The fact that "Filling" is lumped with "Empty" in most of our
//verbiage is pretty gross but I'm not sure of a solution which doesn't
//introduce new, less idiomatic language, or language which is overly 
//verbose ("not ready"/preparing/"empty or filling"
struct BlockState{enum Enum {Empty, Filling, Full};};

//Could be very interesting to use CompleteRays here...
//RayBlocks ARE NOT undifferentiated when empty;
//They should remain coherent or incoherent so that the base ray block count
//remains the same. No need to even keep track of this count
//Then again, what we are actually interested in is that we control memory usage
//So reusing RayBlocks seems desirable. This effects our implementation quite a bit.
//We still want to be able to make new RayBlocks at will for generated rays, but we
//want to cap how many RayBlocks exist to below some constant number. In particular,
//we cannot simply refill a block once it becomes available because this paired with
//block creation when there are no available blocks results in infinite expansion.

//We want to have a way to test out ray block sizes and tile sizes independently
//We can at least use a multiple pretty easily
struct RayBlock
{
	RTCRay rays[RAY_BLOCK_SIZE];
	RayAncillaries ray_ancillariess[RAY_BLOCK_SIZE];
	//RTCRay *rays;
	//RayAncillaries *ray_ancillariess;
	int front_index;

	BlockState::Enum state;
	//bool is_checked_out;//no longer necessary since blocks are literally checked out now
	bool is_primary;//may be replaced, just a note that this sort of thing is necessary
	bool is_coherent;
	//vector<float> occlusion;
	

	//RayBlock(bool is_primary, bool is_coherent, RTCRay *rays, RayAncillaries *ray_ancillariess);
	RayBlock(bool is_primary, bool is_coherent);
	RayBlock();

	void Empty();

	//want to switch this to complete rays
	//Want to actually use this!
	//Make sure to make this use references
	//void AddRay(RTCRay ray, RayAncillaries ray_ancillaries);//should be as fast as accessing the memory directly, but not sure
};

/*#define RAY_PACKET_BLOCK_SIZE (128/ PACKET_SIZE)
struct RayPacketBlock
{
	RTCRayPacket ray_packets[RAY_BLOCK_SIZE];
	RayAncillaries ray_ancillariess[RAY_BLOCK_SIZE];
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

struct Surface
{
	Vec3f position;
	Vec3f normal;

	Surface(Vec3f position, Vec3f normal);
	Surface();
};

//May want to combine occlusions and lights into one object

//Need to pass in space for new rays, but problem is this function can't get a new container if they fill up
//the one given. Could simply return rays, but returning a CompleteRay doesn't work because it assumes there 
//is some underlying storage object for rays and ancillaries. 
void ShadingKernel(RTCRay &ray, /*Surface &surface, float *occlusions,*/ Film &film, RayAncillaries &ray_ancillaries, Scene &scene);//Eventually want to use CompleteRays

struct JobType{enum Enum {None, Refill, Shade};};

struct Job
{
	JobType::Enum type;

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

	Job(JobType::Enum type, RayBlock *ray_block);
	Job();
};

//Note that in practice, each thread basically runs on its own block. It grabs one randomly,
//But then it refills it, and in the microseconds after he returns it, he's the only one looking for
//a new job, and the only available job is the filled block it just turned in.
//I'm suggesting a new system where threads run in their own lanes and less communication is necessary.
//We'd still need a pool of blocks for non-primary rays though, so you'd have to work out how that fits.
//Also note that I'm thinking that atomics are better than mutices if you do it right, since they are incredibly fast.
int primary_ray_block_count= THREAD_COUNT* 1;
class ShaderWorkPool//should think if we can generalize this class
{
	//RTCRay rays[RAY_BLOCK_SIZE* THREAD_COUNT* 1];//This may eventually make more sense to belong to RayBlock as static member
	//RayAncillaries ray_ancillariess[RAY_BLOCK_SIZE* THREAD_COUNT* 1];//and this of course

	queue<RayBlock *> empty_primary_ray_blocks; 
	queue<RayBlock *> empty_coherent_ray_blocks; 
	queue<RayBlock *> empty_incoherent_ray_blocks; 
	queue<RayBlock *> full_ray_blocks;

	std::thread threads[THREAD_COUNT];

	Scene *scene;
	Camera *camera;
	Image *image;

	bool ray_source_is_empty;

	std::mutex job_mutex;
	std::mutex resource_mutex;

protected:
	RayBlock * TakeEmptyPrimaryRayBlock();
	RayBlock * TakeEmptyCoherentRayBlock();
	RayBlock * TakeEmptyIncoherentRayBlock();
	RayBlock * TakeFullRayBlock();
	void ReturnRayBlock(RayBlock *ray_block);

public:
	ShaderWorkPool(Scene *scene, Camera *camera, Image *image);

	void Refill(RayBlock *primary_ray_block);
	void Shade(RayBlock *ray_block);
	bool Develop();

	Job GetJob();

	void WorkLoop();
	void DevelopLoop();

	//This whole class (and several others) need some renaming
	//I'd like to somehow enforce the camera idiom and make this "TakePicture" (obviously that's a camera method, 
	//but somehow we want to integrate things so we don't have this gross low level class, or at least we don't
	//have to interact with it)
	//Also, the name doesn't imply an atomic action (E.g. "Render", "DoWork", etc.) which is the real problem here.
	void SpawnWorkers();
	

	//int GetThreadIndex();
};