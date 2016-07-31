#include "Main.h"

Light::Light()
{

}

bool Light::IsAmbient() const
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


AmbientLight::AmbientLight(Color intensity_)
{
	intensity= intensity_;
}

bool AmbientLight::IsAmbient() const
{
	return true;
}

Color AmbientLight::GetLuminosity(Vec3f point) const
{
	return intensity;
}


bool ShadowLight::IsAmbient() const
{
	return false;
}

Vec3f ShadowLight::SampleDirectionAtPoint(Vec3f point, int sample_index) const
{
	return Vec3f(0, 0, 0);
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

Color PointLight::GetLuminosity(Vec3f point) const
{
	return intensity;//no falloff for now
}

Vec3f PointLight::SampleDirectionAtPoint(Vec3f point, int sample_index) const
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
	return intensity;//no falloff for now
}

Vec3f DiscLight::SampleDirectionAtPoint(Vec3f point, int sample_index) const
{
	return ShadowLight::SampleDirectionAtPoint(point, sample_index);
}


Scene::Scene()
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
		shadow_lights.push_back(dynamic_cast<ShadowLight *>(light));
}

vector<ShadowLight *> * Scene::GetShadowLights()
{
	return &shadow_lights;
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
	pixels= new Pixel[width* height];
}

Image::~Image()
{
	delete pixels;
}


Film::Film(int width_, int height_)
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

//ISPC here
void Film::Develop(Image &image)
{
	float max_component= 0;
	for(int i= 0; i< width* height; i+= 50)
	{
		max_component= std::max(receptors[i].x, max_component);
		max_component= std::max(receptors[i].y, max_component);
		max_component= std::max(receptors[i].z, max_component);
	}
	max_component*= 1.1f;

	for(int i= 0; i< width* height; i++)
	{
		image.pixels[i].r= (unsigned char)((receptors[i].x/ max_component)* 255);
		image.pixels[i].g= (unsigned char)((receptors[i].y/ max_component)* 255);
		image.pixels[i].b= (unsigned char)((receptors[i].z/ max_component)* 255);
	}
}


Camera::Camera(float fov_in_degrees, Vec3f position_, Vec3f direction_)
{
	fov= 3.14f* (fov_in_degrees/ 180.0f);
	position= position_;
	forward= direction_;

	ComputeViewPlane();

	next_tile_index= 0;
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

	view_plane_u= right* tan(fov/ 2);//using aspect ratio of 1
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

void Camera::Reset()
{
	film->Clear();
	next_tile_index= 0;
}

//Shouldn't this be capable of being vectorized?
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

//Doesn't support counts not equal to tile size
//Should be possible to use ISPC
//Could store all rays in memory, then just copy memory
//could try unrolling loop
//The inner loop happens so many times, even tiny optimizations help,
//We should look into using vector intrinsics for the vector math in this inner loop,
//as well as any others in the program

//need a way to  only return count # of rays without breaking parallelism
//Does this really matter? It seems that tile size just affects overhead/job ratio 
//(could be wrong, e.g. 32 and 64 wide better), so couldn't we just make sure the job size was good?
//This still ties ray block sizes to GetRays() counts, but maybe we could just scale up the number of blocks given
//On a refill job, so you could try out small blocks+ big GetRays() count?
void Camera::GetRays(CompleteRay first_ray, int &count)
{
	count= 0;//Hack, really. Supposed to use this to determine how many we generate. (and to say how many we actually did)

	int tile_index= next_tile_index++;//will want to test this to see that it indeed works as expected
	int tile_count_x= film->width/ TILE_WIDTH;//May be best to compute this only once
	int tile_x= tile_index% tile_count_x;
	int tile_y= tile_index/ tile_count_x;

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

			next_ray.ray_ancillaries->absorption= Color(1.0f, 1.0f, 1.0f);
			next_ray.ray_ancillaries->bounce_count= 0;
			next_ray.ray_ancillaries->type= RayType::Primary;
			next_ray.ray_ancillaries->x= x;
			next_ray.ray_ancillaries->y= y;

			next_ray.ray++;
			next_ray.ray_ancillaries++;
		}
	}
}


RayAncillaries::RayAncillaries(float x_, float y_, Color absorption_, int bounce_count_, RayType::Enum type_)
{
	x= x_;
	y= y_;
	absorption= absorption_;
	bounce_count= bounce_count_;
	type= type_;
}

RayAncillaries::RayAncillaries()
{

}

CompleteRay::CompleteRay(RTCRay *ray_, RayAncillaries *ray_ancillaries_)
{
	ray= ray_;
	ray_ancillaries= ray_ancillaries_;
}


RayBlock::RayBlock(bool is_primary_, bool is_coherent_, RTCRay *rays_, RayAncillaries *ray_ancillariess_)
{
	is_primary= is_primary_;
	is_coherent= is_coherent_;
	rays= rays_;
	ray_ancillariess= ray_ancillariess_;

	Empty();
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

/*void RayBlock::AddRay(RTCRay ray, RayAncillaries ray_ancillaries)
{
	rays[front_index]= ray;
	ray_ancillariess[front_index++]= ray_ancillaries;
}*/


Surface::Surface(Vec3f position_, Vec3f normal_)
{
	position= position_;
	normal= normal_;
}

Surface::Surface()
{

}


//It feels wrong that occlusions is passed in separately though it is a direct map to a very specific subset of lights present in Scene
//Interesting to see if theres any difference between reference vs pointer to film
//Needless to say, this function signature is pretty bad and needs to be cleaned up and standardized (don't use const refs anywhere else)
void ShadingKernel(RTCRay &ray, /*Surface &surface, float *occlusions,*/ Film &film, RayAncillaries &ray_ancillaries, Scene &scene)
{
	int x= ray_ancillaries.x;
	int y= ray_ancillaries.y;

	//Looking into filtering out calls like this already, but still want to note that this looks like really bad divergence
	if(ray.geomID== RTC_INVALID_GEOMETRY_ID)
		return;

	Color color= Color(0, 0, 0);

	Surface surface(MakeVec3f(ray.org)+ (MakeVec3f(ray.dir)* ray.tfar), normalize(-MakeVec3f(ray.Ng)));

	vector<ShadowLight *> *shadow_lights= scene.GetShadowLights();
	for(unsigned int i= 0; i< shadow_lights->size(); i++)
	{
		//geometry term is now wrapped in occlusions
		float geometry_term= dot(surface.normal, normalize(-(*shadow_lights)[i]->SampleDirectionAtPoint(surface.position, 0)));
		//float foo_term= occlusions[i]* geometry_term;
		float foo_term= geometry_term;
		if(foo_term<= 0)
			continue;
		
		color+= (*shadow_lights)[i]->GetLuminosity(surface.position)* foo_term;
	}

	vector<AmbientLight *> *ambient_lights= scene.GetAmbientLights();
	for(unsigned int i= 0; i< ambient_lights->size(); i++)
		color+= (*ambient_lights)[i]->GetLuminosity(surface.position);

	film.Stimulate(ray_ancillaries.x, ray_ancillaries.y, color* ray_ancillaries.absorption);
}


Job::Job(JobType::Enum type_, RayBlock *ray_block)
{
	type= type_;

	switch(type)
	{
	case JobType::Refill: 
		refill.primary_ray_block= ray_block; 
		break;

	case JobType::Shade:
		shade.ray_block= ray_block;
		break;

	default:
		type= JobType::None;
		refill.primary_ray_block= nullptr;
	}
}

Job::Job()
{
	type= JobType::None;
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

//this will need to change if we alter how RayBlocks are stored or created
//Starting to think this needs to be a member function...
RayBlock * PopFrontOrInstanciate(queue<RayBlock *> &ray_blocks, std::mutex &mutex, bool is_primary, bool is_coherent)
{
	RayBlock *ray_block= PopFront(ray_blocks, mutex);

	if(ray_block== nullptr)
		return new RayBlock(is_primary, is_coherent, nullptr, nullptr);

	return ray_block;
}

RayBlock * ShaderWorkPool::TakeEmptyPrimaryRayBlock()
{
	return PopFront(empty_primary_ray_blocks, resource_mutex);
}

RayBlock * ShaderWorkPool::TakeEmptyCoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_coherent_ray_blocks, resource_mutex, false, true);
}

RayBlock * ShaderWorkPool::TakeEmptyIncoherentRayBlock()
{
	return PopFrontOrInstanciate(empty_incoherent_ray_blocks, resource_mutex, false, false);
}

RayBlock * ShaderWorkPool::TakeFullRayBlock()
{
	return PopFront(full_ray_blocks, resource_mutex);
}

void ShaderWorkPool::ReturnRayBlock(RayBlock *ray_block)
{
	resource_mutex.lock();

	if(ray_block->state== BlockState::Full)
		full_ray_blocks.push(ray_block);
	else if(ray_block->is_primary)
		empty_primary_ray_blocks.push(ray_block);
	else if(ray_block->is_coherent)
		empty_coherent_ray_blocks.push(ray_block);
	else
		empty_incoherent_ray_blocks.push(ray_block);

	resource_mutex.unlock();
}

ShaderWorkPool::ShaderWorkPool(Scene *scene_, Camera *camera_)
{
	scene= scene_;
	camera= camera_;

	ray_source_is_empty= false;

	for(int i= 0; i< primary_ray_block_count; i++)
		empty_primary_ray_blocks.push(new RayBlock(true, true, &(rays[i* RAY_BLOCK_SIZE]), &(ray_ancillariess[i* RAY_BLOCK_SIZE])));
}

void ShaderWorkPool::Refill(RayBlock *primary_ray_block)
{
	CompleteRay complete_ray= CompleteRay(&primary_ray_block->rays[primary_ray_block->front_index], &primary_ray_block->ray_ancillariess[primary_ray_block->front_index]);
	int count= RAY_BLOCK_SIZE;
	camera->GetRays(complete_ray, count);
	primary_ray_block->front_index+= count;

	primary_ray_block->is_coherent= true;
	if(primary_ray_block->front_index== 0)
	{
		primary_ray_block->state= BlockState::Empty;
		ray_source_is_empty= true;
	}
	if(primary_ray_block->front_index< (RAY_BLOCK_SIZE- 1))
		primary_ray_block->state= BlockState::Filling;
	else
		primary_ray_block->state= BlockState::Full;

	ReturnRayBlock(primary_ray_block);
}

void Foo(ShaderWorkPool *bar)
{
	bar->WorkLoop();
}

//We are currently computing occlusions for failed hits...
void ShaderWorkPool::Shade(RayBlock *ray_block)
{
	RTCIntersectContext context;
	context.flags= ray_block->is_coherent ? RTC_INTERSECT_COHERENT : RTC_INTERSECT_INCOHERENT;
	context.userRayExt= nullptr;
	//rtcIntersect1M(scene->GetEmbreeScene(), nullptr, ray_block->rays, RAY_BLOCK_SIZE, 0);//test out striding ancillaries into array
	for(int i= 0; i< RAY_BLOCK_SIZE; i++)//testing out non-stream mode
		rtcIntersect(scene->GetEmbreeScene(), ray_block->rays[i]);

	/*vector<ShadowLight *> *shadow_lights= scene->GetShadowLights();
	Surface surfaces[RAY_BLOCK_SIZE];
	//vector<RTCRay> shadow_rays;//try making this a fixed sized maximum size array
#define light_count 1 
	RTCRay shadow_rays[RAY_BLOCK_SIZE* light_count];//If using hard lights, we know exactly how many shadow rays needed (not necessarily at compile time)
	//float occlusions[RAY_BLOCK_SIZE* light_count];
	
	int next_shadow_ray_index= 0;
	int next_surface_index= 0;
	for(unsigned int i= 0; i< ray_block->front_index; i++)
	{
		RTCRay *ray= &ray_block->rays[i];//try out making this a value type

		if(ray->geomID!= RTC_INVALID_GEOMETRY_ID);
		else
			continue;

		surfaces[next_surface_index]= Surface(MakeVec3f(ray->org)+ MakeVec3f(ray->dir)* ray->tfar, normalize(-MakeVec3f(ray->Ng)));
		//Vec3f surface_position= MakeVec3f(ray->org)+ (MakeVec3f(ray->dir)* ray->tfar);
		//Vec3f surface_normal= normalize(-MakeVec3f(ray->Ng));

		//Interesting to know whether calculating these surface values once and storing until we 
		//call the shading kernel is faster than just calculating on the fly
		
		//Vec3f surface_position= MakeVec3f(ray->org)+ MakeVec3f(ray->dir)* ray->tfar;
		//Vec3f surface_normal= normalize(-MakeVec3f(ray->Ng));

		//need to think about how we should deal with soft lights...
		//They could have variable number of samples so putting them directly together makes a mess.
		//Since we can divide them into edge and non edge pixels (where edge pixels all have x samples, and non-edge y)
		//We could simply have two arrays, but then you still need a map between the hits and the shadow rays, 
		//because they'd be out-of-order
		//curious to know whether storing result of size() is better...
		for(unsigned int j= 0; j< shadow_lights->size(); j++)
		{
			RTCRay *shadow_ray= &(shadow_rays[next_shadow_ray_index]);//try making this value type

			//Vec3f direction= -(*shadow_lights)[j]->SampleDirectionAtPoint(surfaces[next_surface_index].position, 0);
			//occlusions[next_shadow_ray_index]= dot(surfaces[next_surface_index].normal, normalize(direction));

			SetFloat3(shadow_ray->org, surfaces[next_surface_index].position+ surfaces[next_surface_index].normal* 0.001f);//May want to standardize with epsilon value
			SetFloat3(shadow_ray->dir, -(*shadow_lights)[j]->SampleDirectionAtPoint(surfaces[next_surface_index].position, 0));
			//SetFloat3(shadow_rays->org, surface_position+ surface_normal* 0.001f);//May want to standardize with epsilon value
			//SetFloat3(shadow_rays->dir, -(*shadow_lights)[j]->SampleDirectionAtPoint(surface_position, 0));

			shadow_ray->tnear = 0.0f;
			shadow_ray->tfar = FLT_MAX;
			shadow_ray->geomID = RTC_INVALID_GEOMETRY_ID;
			shadow_ray->primID = RTC_INVALID_GEOMETRY_ID;
			shadow_ray->mask = -1;
			shadow_ray->time = 0;

			next_shadow_ray_index++;
		}

		next_surface_index++;
	}*/

	//try switching to incoherent on shadow rays
	//rtcOccluded1M(scene->GetEmbreeScene(), &context, shadow_rays, RAY_BLOCK_SIZE* light_count, 0);
	/*for(int i= 0; i< next_shadow_ray_index; i++)//testing out non-stream mode
	{
		rtcOccluded(scene->GetEmbreeScene(), shadow_rays[i]);
		//occlusions[i]*= (shadow_rays[i].tfar < 1.0f ? 0.0f : 1.0f);
	}*/

	int rays_processed_count= 0;
	for(unsigned int i= 0; i< ray_block->front_index; i++)
	{
		if(ray_block->rays[i].geomID== RTC_INVALID_GEOMETRY_ID)
			continue;

		/*float occlusions[light_count];
		for(unsigned int j= 0; j< light_count; j++)
			occlusions[j]= shadow_rays[rays_processed_count* light_count+ j].tfar< 1.0f ? 0.0f : 1.0f;*/

		//ShadingKernel(ray_block->rays[i], occlusions, *camera->film, ray_block->ray_ancillariess[i], *scene);
		ShadingKernel(ray_block->rays[i], /*occlusions, surfaces[rays_processed_count],*/ *camera->film, ray_block->ray_ancillariess[i], *scene);
		rays_processed_count++;
	}

	ray_block->Empty();
	ReturnRayBlock(ray_block);

#undef light_count
}


//rewrite this function

//First job refill, or first job complete non-primary blocks?
//The latter makes sure that all entropy info is gatherered before more queries,
//But since the refill looks difficult to paralellize, we may not want the buffer
//to get that small. On the other hand, the cost of generating rays should 
//be << the cost of tracing and shading them
Job ShaderWorkPool::GetJob()
{
	Job job= Job(JobType::None, nullptr);

	job_mutex.lock();

	//see note above RayBlock
	RayBlock *primary_ray_block= nullptr;
	if(!ray_source_is_empty)
		primary_ray_block= TakeEmptyPrimaryRayBlock();
	if(primary_ray_block!= nullptr)
		job= Job(JobType::Refill, primary_ray_block);
	else
	{
		RayBlock *ray_block= TakeFullRayBlock();

		if(ray_block!= nullptr)
			job= Job(JobType::Shade, ray_block);
	}

	job_mutex.unlock();

	return job;
}

//No sure about this/above functions... Seems like they are an uneccessary division.
//But I like the idea of getting a job as a separate process. Problem is that this means
//Giving the work data to the loop, which it doesn't really handle, it just passes it to
//the function it maps to. This becomes awkward when the object has final steps needed which 
//dont fit under the called function's responsibilities. i.e. Shade() should just shade, 
//instead, it also returns the object given to _WorkLoop_ (as a responsibility). This seems wrong
//Because if WorkLoop was given that object, it makes sense for it to also carry out returning it,
//But its discordent for the simplistic, clean WorkLoop to have lower level function calls going on. 
void ShaderWorkPool::WorkLoop()
{
	while(true)
	{
		Job job= GetJob();

		switch(job.type)
		{
		case JobType::Refill: 
			Refill(job.refill.primary_ray_block);
			break;
		case JobType::Shade: 
			Shade(job.shade.ray_block);
			break;

		case JobType::None:
			return;

		default: break;
		}
	}
}

void ShaderWorkPool::SpawnWorkers()
{
	ray_source_is_empty= false;

	//Spawn a bunch of threads, call WorkLoop()
	//parallel_for(size_t(0),size_t(numTilesX*numTilesY),[&](const range<size_t>& range) {

	WorkLoop();

	/*for(int i= 0; i< THREAD_COUNT; i++)
	{
		//cout << this << endl;
		//threads[i]= std::thread(&ShaderWorkPool::WorkLoop, this);
		threads[i]= std::thread(Foo, this);
	}

	for(int i= 0; i< THREAD_COUNT; i++)
		threads[i].join();*/
}


//timing needs redesign
int main(int argument_count, char **arguments)
{
	//I think we need some sort of structure for the global variables
	//these functions create. Right now they're just sorta invisibly floating around.
	InitializeEmbree();
	InitializeOpenGL();

	cout << std::fixed << std::setprecision(1);

	Scene scene;
	scene.AddOBJ("teapot3.obj");
	scene.AddLight(new AmbientLight(Color(0.5f, 0.5f, 0.5f)));
	scene.AddLight(new PointLight(Vec3f(0.0f, 0.0f, 0.0f), Color(0.5f, 0.5f, 0.5f)));
	scene.Commit();

	Camera camera= Camera(90, Vec3f(0.0f, 0.0f, -3.0f));
	camera.LookAt(Vec3f(0.0f, 0.0f, 10.0f));
	camera.LoadFilm(new Film(screen_width, screen_height));

#if 1
	ShaderWorkPool shader_work_pool(&scene, &camera);

	Image image(screen_width, screen_height);
	int start_ticks= SDL_GetTicks();
	int last_ticks= start_ticks;
	int total_frame_count= 0;
	int print_frame_count= 40;
	int total_render_milliseconds= 0;
	while(true)//want framerate limit
	{
		int before_render_ticks= SDL_GetTicks();
		shader_work_pool.SpawnWorkers();
		camera.film->Develop(image);
		total_render_milliseconds+= SDL_GetTicks()- before_render_ticks;
		if((++total_frame_count% print_frame_count)== 0)
		{
			float average_frametime= total_render_milliseconds/ (float)print_frame_count;
			cout << "Render FT/FR: " << 1000/ average_frametime << "fps/" << average_frametime << "ms";
			total_render_milliseconds= 0;
		}

		camera.Reset();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screen_width, screen_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image.pixels);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, quad_vertex_count);
		SDL_GL_SwapWindow(main_window);

		
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

		if((total_frame_count% (print_frame_count* 5))== 0)
			break;
	}

	//We'd like for this to happen as part of ConcludeEmbree. Or at least be automatic.
	//Not sure if we can delete the scene last though
	rtcDeleteScene(scene.GetEmbreeScene());
	ConcludeEmbree();

	return 0;

#else
	Vec3f light_position(0.0f, 0.0f, 0.0f);

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
		SDL_GL_SwapWindow(main_window);

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
	rtcDeleteDevice(device);
	delete image;
	return 0;
#endif
}