#include "Main.h"
#include "ISPCKernels.h"


bool System::systems_initialized= false;
EmbreeSystem System::embree;
GraphicsSystem System::graphics;

void System::InitializeSystems()
{
	embree.Initialize();
	graphics.Initialize();
}

void System::TerminateSystems()
{
	graphics.Terminate();
	embree.Terminate();
}


void EmbreeSystem::Initialize()
{
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	device = rtcNewDevice(nullptr);
	ErrorHandler(rtcDeviceGetError(device), "creation");
	rtcDeviceSetErrorFunction(device,ErrorHandler);
}

void EmbreeSystem::Terminate()
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

void GraphicsSystem::Initialize()
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

void GraphicsSystem::Terminate()
{
	
}


Light::Light()
{

}

bool Light::IsAmbient()
{
	return false;
}

bool Light::IsSoft()
{
	return false;
}

bool Light::IsGenerative()
{
	return false;
}

Vec3f Light::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return Vec3f(0, 0, 0);
}


AmbientLight::AmbientLight(Color intensity_)
{
	intensity= intensity_;
}

bool AmbientLight::IsAmbient()
{
	return true;
}

Color AmbientLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

PointLight::PointLight(Vec3f position_, Color intensity_)
{
	position= position_;
	intensity= intensity_;
}

bool PointLight::IsGenerative()
{
	return true;
}

Color PointLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

Vec3f PointLight::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return point- position;
}


DiscLight::DiscLight(Vec3f position, Color intensity, float radius_)
	: PointLight(position, intensity)
{
	radius= radius_;
}

bool DiscLight::IsSoft()
{
	return true;
}

Color DiscLight::GetLuminosity(Vec3f point)
{
	return intensity;
}

Vec3f DiscLight::SampleDirectionAtPoint(Vec3f point, int sample_index)
{
	return Vec3f(0, 0, 0);
}


Scene::Scene(RTCDevice device)
{
	//need to determine if we need a second stream for packeted intersection
	//interesting to know (as well) whether the stream mode doesn't like switching between packets and singles
	embree_scene = rtcDeviceNewScene(device, RTC_SCENE_STATIC, RTC_INTERSECT1);
}

void Scene::AddOBJ(string filename)
{
	Mesh raw_mesh= LoadOBJ(filename);
	unsigned int geometry_id= rtcNewTriangleMesh(embree_scene, RTC_GEOMETRY_STATIC, raw_mesh.TriangleCount(), raw_mesh.VertexCount());
	Vertex *vertices= (Vertex *)rtcMapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	for(unsigned int i= 0; i< raw_mesh.VertexCount(); i++)
	{
		vertices[i].x= raw_mesh.positions[i* 3+ 0];
		vertices[i].y= raw_mesh.positions[i* 3+ 1];
		vertices[i].z= raw_mesh.positions[i* 3+ 2];
		vertices[i].w= 0;
	}
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_VERTEX_BUFFER);
	Triangle *triangles= (Triangle *)rtcMapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);
	memcpy(triangles, &raw_mesh.position_indices[0], sizeof(int)* raw_mesh.TriangleCount()* 3);
	rtcUnmapBuffer(embree_scene, geometry_id, RTC_INDEX_BUFFER);

	geometry_ids.push_back(geometry_id);
}

void Scene::AddLight(Light *light)
{
	if(light->IsAmbient())
		ambient_lights.push_back(dynamic_cast<AmbientLight *>(light));
	else
		lights.push_back(light);
}

vector<Light *> * Scene::GetLights()
{
	return &lights;
}

vector<AmbientLight *> * Scene::GetAmbientLights()
{
	return &ambient_lights;
}

void Scene::Commit()
{
	rtcCommit(embree_scene);
}

RTCScene Scene::GetEmbreeScene()
{
	return embree_scene;
}


Image::Image(int width, int height)
{
	pixels= nullptr;
	Resize(width, height);
}

Image::Image()
{
	pixels= nullptr;
}

Image::~Image()
{
	if(pixels!= nullptr)
		delete pixels;
}

void Image::Resize(int width, int height)
{
	if(pixels!= nullptr)
		delete pixels;

	pixels= new Pixel[width* height];
}


Film::Film(int width_, int height_)
	: image(width_, height_)
{
	width= width_;
	height= height_;

	receptors= new Vec3f[width* height];
	Clear();
}

Film::~Film()
{
	delete receptors;
}

void Film::Clear()
{
	for(int i= 0; i< width* height; i++)
		receptors[i]= Vec3f(0, 0, 0);
}

void Film::Stimulate(int x, int y, Color light)
{
	receptors[x+ width* y]+= light;
}

void Film::Develop()
{
#if ISPC_DEVELOP
	ispc::Develop(reinterpret_cast<float *>(receptors), reinterpret_cast<int8_t *>(image.pixels), 1, width* height* 3);

#else
	for(int i= 0; i< width* height; i++)
	{
		image.pixels[i].r= (unsigned char)((receptors[i].x)* 255);
		image.pixels[i].g= (unsigned char)((receptors[i].y)* 255);
		image.pixels[i].b= (unsigned char)((receptors[i].z)* 255);
	}

#endif
}

bool Film::Develop_Parallel(int interval_index)
{
	if(interval_index>= (width* height)/ FILM_INTERVAL_SIZE)
		return false;

#if ISPC_DEVELOP
	ispc::Develop(reinterpret_cast<float *>(&(receptors[interval_index* FILM_INTERVAL_SIZE])), reinterpret_cast<int8_t *>(&(image.pixels[interval_index* FILM_INTERVAL_SIZE])), 1, FILM_INTERVAL_SIZE* 3);

#else
	Pixel *pixels= &(image.pixels[interval_index* FILM_INTERVAL_SIZE]);
	Vec3f *receptors= &(this->receptors[interval_index* FILM_INTERVAL_SIZE]);

	for(int i= 0; i< FILM_INTERVAL_SIZE; i++)
	{
		pixels[i].r= (unsigned char)(receptors[i].x* 255);
		pixels[i].g= (unsigned char)(receptors[i].y* 255);
		pixels[i].b= (unsigned char)(receptors[i].z* 255);
	}

#endif

	return true;
}


Camera::Camera(float fov_in_degrees, Vec3f position_, Vec3f direction_)
	: shutter(this)
{
	fov= 3.14f* (fov_in_degrees/ 180.0f);
	position= position_;
	forward= direction_;

	ComputeViewPlane();
}

void Camera::LookAt(Vec3f look_at_position)
{
	forward= normalize(look_at_position- position);
	ComputeViewPlane();
}

void Camera::OrthogonalizeVectors()
{
	up= Vec3f(0, 1, 0);
	forward= normalize(forward);

	right= cross(forward, up);
	up= cross(right, forward);
}

void Camera::ComputeViewPlane()
{
	OrthogonalizeVectors();

	view_plane_u= right* tan(fov/ 2);
	view_plane_v= up* tan(fov/ 2);
}

Film * Camera::LoadFilm(Film *film_)
{
	Film *previous_film= film;
	film= film_;

	return previous_film;
}

Film * Camera::RemoveFilm()
{
	return LoadFilm(nullptr);
}

Image * Camera::TakePicture(Scene &scene)
{
	film->Clear();
	shutter.Open(scene);

	return &(film->image);
}


void SetFloat3(float *destination, const Vec3f &vector)
{
	destination[0]= vector[0];
	destination[1]= vector[1];
	destination[2]= vector[2];
}

//Need to decide on Make/Construct/Build/etc for these sorts of functions
Vec3f MakeVec3f(const float components[3])
{
	return Vec3f(components[0], components[1], components[2]);
}


void Camera::GetRays(CompleteRay first_ray, int &count, int tile_index)
{
#if ISPC_GET_RAYS
	int tile_index= next_tile_index++;
	if(tile_index>= ((film->width/ TILE_WIDTH)* (film->height/ TILE_HEIGHT)))
	{
		count= 0;
		return;
	}

	/*float position_[3]; SetFloat3(position_, position);
	float forward_[3]; SetFloat3(forward_, forward);
	float view_plane_u_[3]; SetFloat3(view_plane_u_, view_plane_u);
	float view_plane_v_[3]; SetFloat3(view_plane_v_, view_plane_v);*/

	int tile_count_x= film->width/ TILE_WIDTH;
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;
	int x_offset= tile_x* TILE_WIDTH;
	int y_offset= tile_y* TILE_HEIGHT;

	
#if 0
	ispc::GetRayDirections(x_offset, y_offset,
							&(position[0]), &(forward[0]), 
							&(view_plane_u[0]), &(view_plane_v[0]),
							film->width, film->height, 
							reinterpret_cast<ispc::RTCRay *>(first_ray.ray));

#else
	float *pos= new float[4]; SetFloat3(pos, position);
	float *forw= new float[4]; SetFloat3(forw, forward);
	float *u= new float[4]; SetFloat3(u, view_plane_u);
	float *v= new float[4]; SetFloat3(v, view_plane_v);

	ispc::GetRayDirections(x_offset, y_offset,
							pos, forw, 
							u, v,
							film->width, film->height, 
							reinterpret_cast<ispc::RTCRay *>(first_ray.ray));

	delete pos, forw, u, v;
#endif
	
	//position_[0]= 0;

#if 1
	
	CompleteRay next_ray= first_ray;
	for(int j= 0; j< TILE_HEIGHT; j++)
	{
		for(int i= 0; i< TILE_WIDTH; i++)
		{
			int x= i+ tile_x* TILE_WIDTH;
			int y= j+ tile_y* TILE_HEIGHT;
			if(x< film->width && y< film->height);
			else
				continue;

			next_ray.ray->tnear = 0.0f;
			next_ray.ray->tfar = FLT_MAX;
			next_ray.ray->geomID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->primID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->mask = -1;
			next_ray.ray->time = 0;

			next_ray.ray_extras->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.ray_extras->bounce_count= 0;
			next_ray.ray_extras->type= RayType::Primary;
			next_ray.ray_extras->x= x;
			next_ray.ray_extras->y= y;

			next_ray.ray++;
			next_ray.ray_extras++;
		}
	}

#else
	count= 0;//Hack, really. Supposed to use this to determine how many we generate. (and to say how many we actually did)
	CompleteRay next_ray= first_ray;
	for(int j= 0; j< TILE_HEIGHT; j++)
	{
		for(int i= 0; i< TILE_WIDTH; i++)
		{
			int x= i+ tile_x* TILE_WIDTH;
			int y= j+ tile_y* TILE_HEIGHT;
			if(x< film->width && y< film->height);
			else
				continue;
			count++;

			
			float normalized_x= ((x+ 0.5f)/ film->width)* 2- 1;
			float normalized_y= ((y+ 0.5f)/ film->height)* 2- 1;

			//SetFloat3(next_ray.ray->org, position);
			//-normalized is quick hack
			//next_ray.ray->dir[0]= forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
			//next_ray.ray->dir[1]= forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
			//next_ray.ray->dir[2]= forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

			next_ray.ray->tnear = 0.0f;
			next_ray.ray->tfar = FLT_MAX;
			next_ray.ray->geomID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->primID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->mask = -1;
			next_ray.ray->time = 0;

			next_ray.ray_extras->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.ray_extras->bounce_count= 0;
			next_ray.ray_extras->type= RayType::Primary;
			next_ray.ray_extras->x= x;
			next_ray.ray_extras->y= y;

			next_ray.ray++;
			next_ray.ray_extras++;
		}
	}
#endif

#else
	if(tile_index>= ((film->width/ CAMERA_TILE_WIDTH)* (film->height/ CAMERA_TILE_HEIGHT)))
	{
		count= 0;
		return;
	}

	//int tile_index= next_tile_index++;//will want to test this to see that it indeed works as expected
	int tile_count_x= film->width/ CAMERA_TILE_WIDTH;//May be best to compute this only once
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

	CompleteRay next_ray= first_ray;
	for(int j= 0; j< CAMERA_TILE_HEIGHT; j++)
	{
		for(int i= 0; i< CAMERA_TILE_WIDTH; i++)
		{
			int x= i+ tile_x* CAMERA_TILE_WIDTH;
			int y= j+ tile_y* CAMERA_TILE_HEIGHT;

			float normalized_x= ((x+ 0.5f)/ film->width)* 2- 1;
			float normalized_y= ((y+ 0.5f)/ film->height)* 2- 1;

			SetFloat3(next_ray.ray->org, position);
			//-normalized is quick hack
			next_ray.ray->dir[0]= forward[0]+ view_plane_u[0]* -normalized_x+ view_plane_v[0]* normalized_y;
			next_ray.ray->dir[1]= forward[1]+ view_plane_u[1]* -normalized_x+ view_plane_v[1]* normalized_y;
			next_ray.ray->dir[2]= forward[2]+ view_plane_u[2]* -normalized_x+ view_plane_v[2]* normalized_y;

			next_ray.ray->tnear = 0.0f;
			next_ray.ray->tfar = FLT_MAX;
			next_ray.ray->geomID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->primID = RTC_INVALID_GEOMETRY_ID;
			next_ray.ray->mask = -1;
			next_ray.ray->time = 0;

			next_ray.extras->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.extras->bounce_count= 0;
			next_ray.extras->type= RayType::Primary;
			next_ray.extras->x= x;
			next_ray.extras->y= y;

			next_ray.ray++;
			next_ray.extras++;
		}
	}

#endif
}


RayExtras::RayExtras(float x_, float y_, Color absorption_, int bounce_count_, RayType::Enum type_)
{
	x= x_;
	y= y_;
	absorption= absorption_;
	bounce_count= bounce_count_;
	type= type_;
}

RayExtras::RayExtras()
{

}

CompleteRay::CompleteRay(RTCRay *ray_, RayExtras *extras_)
{
	ray= ray_;
	extras= extras_;
}


RayBlock::RayBlock(bool is_primary_, bool is_coherent_)
{
	is_primary= is_primary_;
	is_coherent= is_coherent_;

	Empty();
}

RayBlock::RayBlock()
{

}

void RayBlock::Empty()
{
	front_index= 0;
	state= BlockState::Empty;
}


Surface::Surface(Vec3f position_, Vec3f normal_)
{
	position= position_;
	normal= normal_;
}

Surface::Surface()
{

}


void ShadingKernel(CompleteRay ray, /*Surface &surface, float *occlusions,*/ Film *film, Scene *scene)
{
	int x= ray.extras->x;
	int y= ray.extras->y;

	if(ray.ray->geomID== RTC_INVALID_GEOMETRY_ID)
		return;

	Color color= Color(0, 0, 0);

	Surface surface(MakeVec3f(ray.ray->org)+ (MakeVec3f(ray.ray->dir)* ray.ray->tfar), normalize(-MakeVec3f(ray.ray->Ng)));

	vector<Light *> *lights= scene->GetLights();
	for(unsigned int i= 0; i< lights->size(); i++)
	{
		//geometry term is now wrapped in occlusions
		float geometry_term= dot(surface.normal, normalize(-(*lights)[i]->SampleDirectionAtPoint(surface.position, 0)));
		//float foo_term= occlusions[i]* geometry_term;
		float foo_term= geometry_term;
		if(foo_term<= 0)
			continue;
		
		color+= (*lights)[i]->GetLuminosity(surface.position)* foo_term;
	}

	vector<AmbientLight *> *ambient_lights= scene->GetAmbientLights();
	for(unsigned int i= 0; i< ambient_lights->size(); i++)
		color+= (*ambient_lights)[i]->GetLuminosity(surface.position);

	film->Stimulate(ray.extras->x, ray.extras->y, color* ray.extras->absorption);
}


Task::Task(TaskType::Enum type_, RayBlock *ray_block)
{
	type= type_;
	refill.primary_ray_block= nullptr;

	switch(type)
	{
	case TaskType::Refill: 
		refill.primary_ray_block= ray_block; 
		break;

	case TaskType::Shade:
		shade.ray_block= ray_block;
		break;

	default:
		break;
	}
}

Task::Task()
{
	type= TaskType::None;
	refill.primary_ray_block= nullptr;
}

RayBlock * PopFront(queue<RayBlock *> &ray_blocks, std::mutex &mutex)
{
	mutex.lock();

	RayBlock *ray_block= nullptr;

	if(ray_blocks.size()> 0)
	{
		ray_block= ray_blocks.front();
		ray_blocks.pop();
	}

	mutex.unlock();

	return ray_block;
}

RayBlock * PopFrontOrInstanciate(queue<RayBlock *> &ray_blocks, std::mutex &mutex, bool is_primary, bool is_coherent)
{
	RayBlock *ray_block= PopFront(ray_blocks, mutex);

	if(ray_block== nullptr)
		return new RayBlock(is_primary, is_coherent);

	return ray_block;
}


RayBlock * Shutter::TakeEmptyPrimaryRayBlock()
{
	return PopFront(empty_primary_ray_blocks, resource_mutex);
}

RayBlock * Shutter::TakeEmptyCoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_coherent_ray_blocks, resource_mutex, false, true);
}

RayBlock * Shutter::TakeEmptyIncoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_incoherent_ray_blocks, resource_mutex, false, false);
}

RayBlock * Shutter::TakeFullRayBlock()
{
	return PopFront(full_ray_blocks, resource_mutex);
}

void Shutter::ReturnRayBlock(RayBlock *ray_block)
{
	resource_mutex.lock();

	if(ray_block->state== BlockState::Full)
		full_ray_blocks.push(ray_block);
	else
	{
		if(ray_block->is_primary)
			empty_primary_ray_blocks.push(ray_block);
		else if(ray_block->is_coherent)
			empty_coherent_ray_blocks.push(ray_block);
		else
			empty_incoherent_ray_blocks.push(ray_block);
	}

	resource_mutex.unlock();
}

Shutter::Shutter(Camera *camera_)
{
	camera= camera_;

	Reset();

	for(int i= 0; i< THREAD_COUNT; i++)
		empty_primary_ray_blocks.push(new RayBlock(true, true));
}

void Shutter::Refill(RayBlock *primary_ray_block)
{
	CompleteRay complete_ray= CompleteRay(&(primary_ray_block->rays[primary_ray_block->front_index]), &(primary_ray_block->ray_extrass[primary_ray_block->front_index]));
	int count= RAY_BLOCK_SIZE;
	int camera_tile_index= next_camera_tile_index++;
	camera->GetRays(complete_ray, count, camera_tile_index);
	primary_ray_block->front_index+= count;

	primary_ray_block->is_coherent= true;
	if(primary_ray_block->front_index== 0)
	{
		primary_ray_block->state= BlockState::Empty;
		ray_source_exhausted= true;
	}
	if(primary_ray_block->front_index< (RAY_BLOCK_SIZE- 1))
		primary_ray_block->state= BlockState::Partial;
	else
		primary_ray_block->state= BlockState::Full;

	ReturnRayBlock(primary_ray_block);
}

//We are currently computing occlusions for failed hits...
void Shutter::Shade(RayBlock *ray_block, Scene *scene, Film *film)
{
	RTCIntersectContext context;
	context.flags= ray_block->is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;
	//rtcIntersect1M(scene->GetEmbreeScene(), nullptr, ray_block->rays, RAY_BLOCK_SIZE, 0);//test out striding extras into array
	for(int i= 0; i< RAY_BLOCK_SIZE; i++)
		rtcIntersect(scene->GetEmbreeScene(), ray_block->rays[i]);

	int rays_processed_count= 0;
	for(unsigned int i= 0; i< ray_block->front_index; i++)
	{
		if(ray_block->rays[i].geomID== RTC_INVALID_GEOMETRY_ID)
			continue;

		ShadingKernel(CompleteRay(&(ray_block->rays[i]), &(ray_block->ray_extrass[i])), film, scene);
		rays_processed_count++;
	}

	ray_block->Empty();
	ReturnRayBlock(ray_block);

#undef light_count
}

void Shutter::Develop(Film *film)
{
	int film_interval_index= next_film_interval_index++;
	develop_finished= !film->Develop_Parallel(film_interval_index);
}

Task Shutter::GetTask()
{
	Task task= Task(TaskType::None, nullptr);

	task_mutex.lock();//Test out removing this mutex after redesign finished

	RayBlock *primary_ray_block= nullptr;
	if(!ray_source_exhausted)
		primary_ray_block= TakeEmptyPrimaryRayBlock();
	if(primary_ray_block!= nullptr)
		task= Task(TaskType::Refill, primary_ray_block);
	else
	{
		RayBlock *ray_block= TakeFullRayBlock();

		if(ray_block!= nullptr)
			task= Task(TaskType::Shade, ray_block);
#if PARALLEL_DEVELOP
		else 
		{
			if(!develop_finished)
				task= Task(TaskType::Develop);
		}
#endif
	}

	task_mutex.unlock();

	return task;
}

void Shutter::Reset()
{
	ray_source_exhausted= false;
	develop_finished= false;

	next_camera_tile_index= 0;
	next_film_interval_index= 0;

	barrier.Reset();
}

void Shutter::TaskLoop(Scene *scene)
{
	Task task;
	while(task.type!= TaskType::Develop)//Hack, but will be replaced in the future anyways. 
	{
		task= GetTask();

		switch(task.type)
		{
		case TaskType::Refill: 
			Refill(task.refill.primary_ray_block);
			break;
		case TaskType::Shade: 
			Shade(task.shade.ray_block, scene, camera->film);
			break;

		default: break;
		}
	}

	barrier.Wait();

	while(true)
	{
		task= GetTask();

		switch(task.type)
		{
		case TaskType::Develop:
			Develop(camera->film);
			break;

		case TaskType::None:
			return;

		default: break;
		}
	}
}

void Shutter::Open(Scene &scene)
{
	Reset();

#if SERIAL_MODE
	TaskLoop(&scene);

#else
	for(int i= 0; i< THREAD_COUNT; i++)
		threads[i]= std::thread(&Shutter::TaskLoop, this, &scene);
	for(int i= 0; i< THREAD_COUNT; i++)
		threads[i].join();

#endif
}

//timing needs redesign
int main(int argument_count, char **arguments)
{
	System::InitializeSystems();

	cout << std::fixed << std::setprecision(1);

	Scene scene(System::embree.device);
	scene.AddOBJ("teapot3.obj");
	scene.AddLight(new AmbientLight(Color(0.5f, 0.5f, 0.5f)));
	scene.AddLight(new PointLight(Vec3f(0.0f, 0.0f, 0.0f), Color(0.5f, 0.5f, 0.5f)));
	scene.Commit();

	Camera camera(90, Vec3f(0.0f, 0.0f, -3.0f));
	camera.LookAt(Vec3f(0.0f, 0.0f, 10.0f));
	Film film(System::graphics.screen_width, System::graphics.screen_height);
	camera.LoadFilm(&film);

#if 1
	int start_ticks= SDL_GetTicks();
	int last_ticks= start_ticks;
	int total_frame_count= 0;
	int print_frame_count= 40;
	int total_render_milliseconds= 0;
	while(true)
	{
		int before_render_ticks= SDL_GetTicks();
		camera.TakePicture(scene);
#if PARALLEL_DEVELOP
#else
		camera->film->Develop(image);
#endif
		total_render_milliseconds+= SDL_GetTicks()- before_render_ticks;
		if((++total_frame_count% print_frame_count)== 0)
		{
			float average_frametime= total_render_milliseconds/ (float)print_frame_count;
			cout << "Render FT/FR: " << 1000/ average_frametime << "fps/" << average_frametime << "ms";
			total_render_milliseconds= 0;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, System::graphics.screen_width, System::graphics.screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera.film->image.pixels);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count);
		SDL_GL_SwapWindow(System::graphics.main_window);

		
		if((total_frame_count% print_frame_count)== 0)
		{
			int current_ticks= SDL_GetTicks();
			float seconds= (current_ticks- last_ticks)/ 1000.0f;
			cout << ", Display FR/FT/AVFT: " << print_frame_count/ seconds << "fps/" << 1000* seconds/ print_frame_count << "ms/" << (current_ticks- start_ticks)/ (float)total_frame_count << "ms\n";
			last_ticks= current_ticks;
		}
		

		bool exit_requested= false;
		SDL_Event event_;
		while(SDL_PollEvent(&event_))
			if(event_.type== SDL_QUIT)
				exit_requested= true;
		if(exit_requested)
			break;

		if((total_frame_count% (print_frame_count* 15))== 0)
			break;
	}

	//We'd like for this to happen as part of ConcludeEmbree. Or at least be automatic.
	//Not sure if we can delete the scene last though
	rtcDeleteScene(scene.GetEmbreeScene());
	System::TerminateSystems();

	return 0;

#else
	Vec3f light_position(0.0f, 0.0f, 0.0f);

	int screen_width= System::graphics.screen_width;
	int screen_height= System::graphics.screen_height;

	unsigned char *image= new unsigned char[3* screen_width* screen_height];
	//for(int i= 0; i< 3* screen_width* screen_height; i++)
	//	image[i]= 255;

	int last_ticks= SDL_GetTicks();
	int total_frame_count= 0;
	int print_frame_count= 20;
	while(true)
	{
		for(int j= 0; j< screen_height; j++)
		{
			for(int i= 0; i< screen_width; i++)
			{
				image[3* i+ 3* screen_width* j+ 0]= 0;
				image[3* i+ 3* screen_width* j+ 1]= 0;
				image[3* i+ 3* screen_width* j+ 2]= 0;

				float normalized_x= ((i+ 0.5f)/ (float)screen_width)* 2- 1;
				float normalized_y= ((j+ 0.5f)/ (float)screen_height)* 2- 1;

				Vec3f ray_direction= camera.forward+ camera.view_plane_u* normalized_x+ camera.view_plane_v* normalized_y;

				RTCRay ray;
				SetFloat3(ray.org, camera.position);
				SetFloat3(ray.dir, ray_direction);
				//ray.org[0] = camera.position[0]; ray.org[1] = camera.position[1]; ray.org[2] = camera.position[2];
				//ray.dir[0] = ray_direction[0]; ray.dir[1] = ray_direction[1]; ray.dir[2] = ray_direction[2];
				ray.tnear = 0.0f;
				ray.tfar = FLT_MAX;
				ray.geomID = RTC_INVALID_GEOMETRY_ID;
				ray.primID = RTC_INVALID_GEOMETRY_ID;
				ray.mask = -1;
				ray.time = 0;

				rtcIntersect(scene.GetEmbreeScene(), ray);
				Vec3f ray_origin= MakeVec3f(ray.org);
				Vec3f surface_position= ray_origin+ (MakeVec3f(ray.dir)* ray.tfar);
				Vec3f surface_normal= normalize(Vec3f(ray.Ng[0], ray.Ng[1], ray.Ng[2]));

				if(ray.geomID== RTC_INVALID_GEOMETRY_ID)
				{

				}
				else
				{


					Vec3f color(1.0f, 1.0f, 1.0f);
					float diffuse_term= dot(surface_normal, normalize(surface_position- light_position));
					color*= 0.5f+ 0.5f* diffuse_term;
					//color= Vec3f(abs(surface_normal[0]), abs(surface_normal[1]), abs(surface_normal[2]));

					int red_index= 3* i+ 3* screen_width* j;
					image[red_index+ 0]= (unsigned char)(color[0]* 255);
					image[red_index+ 1]= (unsigned char)(color[1]* 255);
					image[red_index+ 2]= (unsigned char)(color[2]* 255);
				}
			
			}
		}

	
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count);
		SDL_GL_SwapWindow(System::graphics.main_window);

		if((++total_frame_count% print_frame_count)== 0)
		{
			int current_ticks= SDL_GetTicks();
			float seconds= (current_ticks- last_ticks)/ 1000.0f;
			cout << "framerate: " << print_frame_count/ seconds << ", frametime: " << 1000* seconds/ print_frame_count << " ms" << endl;
			last_ticks= current_ticks;
		}

		bool exit_requested= false;
		SDL_Event event_;
		while(SDL_PollEvent(&event_))
			if(event_.type== SDL_QUIT)
				exit_requested= true;
		if(exit_requested)
			break;

		if((total_frame_count% (print_frame_count* 5))== 0)
			break;
	}

	rtcDeleteScene(scene.GetEmbreeScene());
	rtcDeleteDevice(System::embree.device);
	delete image;
	return 0;
#endif
}