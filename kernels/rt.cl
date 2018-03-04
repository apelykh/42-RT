/* required to compensate for limited float precision */
// __constant float EPSILON = 0.00003f;
__constant float EPSILON = 0.0003f;
__constant float PI = 3.14159265359f;
__constant int SAMPLES = 500;
__constant int NUM_BOUNCES = 7;

#define PLANE		0
#define SPHERE		1
#define CYLINDER	2
#define CONE		3


typedef struct	s_ray
{
	float3 origin;
	float3 dir;
}				t_ray;

typedef struct	s_matrix
{
	float4		a;
	float4		b;
	float4		c;
	float4		d;
}				t_matrix;

typedef struct	s_object
{
	int type;
	float radius;
	float radius2;
	float k;
	float3 pos;
	float3 dir;
	float3 color;
	float3 emi;
	float diffuse;
	float specular;
	float specular_exp;
}				t_object;

typedef struct	s_light
{
	float3 pos;
	float3 emission;
}				t_light;

typedef struct	s_camera
{
	float3		pos;
}				t_camera;


static float get_random(unsigned int *seed0, unsigned int *seed1);
static float zero_clamp(float x);
t_ray create_cam_ray(__constant t_camera *camera, const int x_coord, const int y_coord, const int width, const int height);
float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int sphere_count, const int *seed0, const int *seed1);
float3 get_direct_light(__constant t_object *spheres, const t_ray *camray, const int sphere_count, __constant t_light *lights, const int num_lights);

// -------------------------- Object intersections ----------------------------
float	intersect_sphere(const t_object *obj, const t_ray *ray);
float	intersect_plane(const t_object *obj, const t_ray *ray);
float	intersect_cylinder(const t_object *obj, const t_ray *ray);
float	intersect_cone(const t_object *obj, const t_ray *ray);
bool	intersect_scene(__constant t_object *objects, const int num_objects,
					const t_ray *ray, float3 *normal, float *t, int *object_id);

// ---------------------------- Object normals --------------------------------
float3	normal_sphere(const t_object *obj, const t_ray *ray, float dist);
float3	normal_plane(const t_object *obj, const t_ray *ray);
float3	normal_cylinder(const t_object *obj, const t_ray *ray, float dist);
float3	normal_cone(const t_object *obj, const t_ray *ray, float dist);

float3		hitpoint_calc(const t_ray *ray, float dist);
t_matrix	init_transform_matrix(float3 camera_pos);
float3		mult_matr_vec(t_matrix m, float3 v);


t_matrix		init_transform_matrix(float3 camera_pos)
{
	t_matrix m;
	float3 e1 = (float3)(1.0f, 0.0f, 0.0f);
	float3 e2 = (float3)(0.0f, 1.0f, 0.0f);
	float3 e3 = (float3)(0.0f, 0.0f, 1.0f);
	float3 t = camera_pos;

	m.a.x = e1.x;
	m.a.y = e1.y;
	m.a.z = e1.z;
	m.a.w = 0;

	m.b.x = e2.x;
	m.b.y = e2.y;
	m.b.z = e2.z;
	m.b.w = 0;

	m.c.x = e3.x;
	m.c.y = e3.y;
	m.c.z = e3.z;
	m.d.w = 0;

	m.d.x = t.x;
	m.d.y = t.y;
	m.d.z = t.z;
	m.d.w = 1;

	return m;
}

float3	mult_matr_vec(t_matrix m, float3 v)
{
	float3 res;

	// since v.w is assumed to always be equal to 1, it needn't be defined explicitly

	res.x = v.x * m.a.x + v.y * m.b.x + v.z * m.c.x + m.d.x;
	res.y = v.x * m.a.y + v.y * m.b.y + v.z * m.c.y + m.d.y;
	res.z = v.x * m.a.z + v.y * m.b.z + v.z * m.c.z + m.d.z;

	// no need to compute w explicitly. It's always equal to 1 for affine transforms
	// w = src.x * 0 + src.y * 0 + src.z * 0 + 1 * 1 = 1;
	return (res);
} 

static float get_random(unsigned int *seed0, unsigned int *seed1)
{
	/* hash the seeds using bitwise AND operations and bitshifts */
	*seed0 = 36969 * ((*seed0) & 65535) + ((*seed0) >> 16);  
	*seed1 = 18000 * ((*seed1) & 65535) + ((*seed1) >> 16);

	unsigned int ires = ((*seed0) << 16) + (*seed1);

	/* use union struct to convert int to float */
	union {
		float f;
		unsigned int ui;
	} res;

	res.ui = (ires & 0x007fffff) | 0x40000000;  /* bitwise AND, bitwise OR */
	return (res.f - 2.0f) / 2.0f;
}

static float zero_clamp(float x) { return (x < 0.0f) ? 0.0f : x; }

float3 hitpoint_calc(const t_ray *ray, float dist)
{
	return ray->origin + ray->dir * dist;
}

t_ray create_cam_ray(__constant t_camera *camera, const int x, const int y,
						const int width, const int height)
{
	float aspect_ratio = (float)(width) / (float)(height);

	float px = (2 * ((x + 0.5) / width) - 1) * 1.0f * aspect_ratio;
	float py = (1 - 2 * ((y + 0.5) / height) * 1.0f);

	float3 origin = (float3)(0.0f, 0.0f, 0.0f);

	t_matrix cam2world = init_transform_matrix(camera->pos);

	float3 origin_world, pixel_pos_world;
	origin_world = mult_matr_vec(cam2world, origin);
	pixel_pos_world = mult_matr_vec(cam2world, (float3)(px, py, -1));

	t_ray ray;
	ray.origin = origin_world;
	ray.dir = normalize(pixel_pos_world - origin_world);

	return ray;
}

/* version using local copy of sphere */
float intersect_sphere(const t_object *obj, const t_ray *ray)
{
	float3 ray_to_center = obj->pos - ray->origin;
	float b = dot(ray_to_center, ray->dir);
	float c = dot(ray_to_center, ray_to_center) - obj->radius * obj->radius;
	float disc = b * b - c;

	if (disc < 0.0f)
		return 0.0f;
	else
		disc = sqrt(disc);

	if ((b - disc) > EPSILON)
		return b - disc;
	if ((b + disc) > EPSILON)
		return b + disc;

	return 0.0f;
}

float3 normal_sphere(const t_object *obj, const t_ray *ray, float dist)
{
	float3 hitpoint = hitpoint_calc(ray, dist);
	float3 normal = fast_normalize(hitpoint - obj->pos); 
	normal = dot(normal, ray->dir) < 0.0f ? normal : normal * (-1.0f);

	return normal;
}

float intersect_plane(const t_object *obj, const t_ray *ray)
{
	float d_v = dot(ray->dir, obj->dir);
	if (d_v != 0.0f)
	{
		float x_v = dot((obj->pos - ray->origin), obj->dir);
		float dist = 0.0f;
		if ((x_v > 0.0 && d_v > 0.0) || (x_v < 0.0 && d_v < 0.0))
			if ((dist = x_v / d_v) > EPSILON)
				return dist;
	}

	return 0.0f;
}

float3 normal_plane(const t_object *obj, const t_ray *ray)
{
	float3 normal = obj->dir;
	normal = dot(normal, ray->dir) < 0.0f ? normal : normal * (-1.0f);

	return normal;
}

float intersect_cylinder(const t_object *obj, const t_ray *ray)
{
	float3 x = obj->pos - ray->origin;
	float d_v = dot(ray->dir, obj->dir);
	float x_v = dot(x, obj->dir);
	float a = 1.0f - d_v * d_v;
	float b = dot(ray->dir, x) - d_v * x_v;
	float c = dot(x, x) - x_v * x_v - (obj->radius * obj->radius);
	float disc = b * b - a * c;

	if (disc < 0.0f)
		return 0.0f;
	else
		disc = sqrt(disc);

	if ((b - disc) / a > EPSILON)
		return (b - disc) / a;
	if ((b + disc) / a > EPSILON)
		return (b + disc) / a;

	return 0.0f;
}

float3 normal_cylinder(const t_object *obj, const t_ray *ray, float dist)
{
	float3 hitpoint = hitpoint_calc(ray, dist);
	
	float m = dot(ray->dir, obj->dir) * dist - dot(obj->pos - ray->origin, obj->dir);
	
	float3 normal = fast_normalize(hitpoint - obj->pos - obj->dir * m);
	return normal;
}

float intersect_cone(const t_object *obj, const t_ray *ray)
{
	float k_k = pow(tan(obj->k * PI / 360), 2);
	float3 x = obj->pos - ray->origin;
	float d_v = dot(ray->dir, obj->dir);
	float x_v = dot(x, obj->dir);
	float a = (1.0f - (1.0f + k_k) * d_v * d_v);
	float b = dot(ray->dir, x) - (1 + k_k) * d_v * x_v;
	float c = dot(x, x) - (1.0f + k_k) * x_v * x_v;
	float disc = b * b - a * c;

	if (disc < 0.0f)
		return 0.0f;
	else
		disc = sqrt(disc);

	if ((b - disc) / a > EPSILON)
		return (b - disc) / a;
	if ((b + disc) / a > EPSILON)
		return (b + disc) / a;

	return 0.0f;
}

float3 normal_cone(const t_object *obj, const t_ray *ray, float dist)
{
	float3 hitpoint = hitpoint_calc(ray, dist);
	float k_k = pow(tan(obj->k * PI / 360), 2);
	
	float m = dot(ray->dir, obj->dir) * dist - dot(obj->pos - ray->origin, obj->dir);

	float3 normal = fast_normalize(hitpoint - obj->pos - obj->dir * m * (1 + k_k));

	return normal;
}

// float3 find_normal(const t_object *obj, const t_ray *ray, float dist)
// {
// 	float3 normal = (float3)(0.0f, 0.0f, 0.0f);

// 	switch(obj->type)
// 	{
// 		case PLANE :
// 			normal = normal_plane(obj, ray);
// 			break;
// 		case SPHERE :
// 			normal = normal_sphere(obj, ray, dist);
// 			break;
// 		case CYLINDER :
// 			normal = normal_cylinder(obj, ray, dist);
// 			break;
// 		case CONE :
// 			normal = normal_cone(obj, ray, dist);
// 			break;
// 	}
// 	return normal;
// }

bool intersect_scene(__constant t_object *objects, const int num_objects,
					const t_ray *ray, float3 *normal, float *t, int *object_id)
{
	/* initialise t to a very large number, 
	so t will be guaranteed to be smaller
	when a hit with the scene occurs */
	float inf = 1e20f;
	float3 loc_normal = (float3)(0.0f, 0.0f, 0.0f);
	*t = inf;

	/* check if the ray intersects each object in the scene */
	for (int i = 0; i < num_objects; i++)
	{
		/* create local copy of sphere */
		t_object obj = objects[i];

		float hitdistance = 0.0f;
		switch(obj.type)
		{
			case PLANE :
				hitdistance = intersect_plane(&obj, ray);
				loc_normal = normal_plane(&obj, ray);
				break;
			case SPHERE :
				hitdistance = intersect_sphere(&obj, ray);
				loc_normal = normal_sphere(&obj, ray, hitdistance);
				break;
			case CYLINDER :
				hitdistance = intersect_cylinder(&obj, ray);
				loc_normal = normal_cylinder(&obj, ray, hitdistance);
				break;
			case CONE :
				hitdistance = intersect_cone(&obj, ray);
				loc_normal = normal_cone(&obj, ray, hitdistance);
				break;
		}
		/* keep track of the closest intersection and hitobject found so far */
		if (hitdistance != 0.0f && hitdistance < *t)
		{
			*t = hitdistance;
			*normal = loc_normal;
			*object_id = i;
		}
	}
	/* true when ray interesects the scene */
	return *t < inf;
}


/* the path tracing function */
/* computes a path (starting from the camera) with a defined number of bounces, accumulates light/color at each bounce */
/* each ray hitting a surface will be reflected in a random direction (by randomly sampling the hemisphere above the hitpoint) */
/* small optimisation: diffuse ray directions are calculated using cosine weighted importance sampling */

float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int sphere_count, const int *seed0, const int *seed1)
{
	t_ray ray = *camray;

	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 mask = (float3)(1.0f, 1.0f, 1.0f);
	float3 normal_facing = (float3)(0.0f, 0.0f, 0.0f);

	for (int bounces = 0; bounces < NUM_BOUNCES; bounces++)
	{
		float t;   /* distance to intersection */
		int hitobject_id = 0; /* index of intersected sphere */

		/* if ray misses scene, return background colour */
		if (!intersect_scene(spheres, sphere_count, &ray, &normal_facing, &t, &hitobject_id))
			return accum_color += mask * (float3)(0.15f, 0.15f, 0.25f);

		/* else, we've got a hit! Fetch the closest hit sphere */
		t_object hitobject = spheres[hitobject_id]; /* version with local copy of sphere */

		/* compute the hitpoint using the ray equation */
		float3 hitpoint = ray.origin + ray.dir * t;
		
		/* compute the surface normal and flip it if necessary to face the incoming ray */
		// float3 normal_facing = find_normal(&hitobject, &ray, t);

		/* compute two random numbers to pick a random point on the hemisphere above the hitpoint */
		float rand1 = 2.0f * PI * get_random(seed0, seed1);
		float rand2 = get_random(seed0, seed1);
		float rand2s = sqrt(rand2);

		/* create a local orthogonal coordinate frame centered at the hitpoint */
		float3 w = normal_facing;
		float3 axis = fabs(w.x) > 0.1f ? (float3)(0.0f, 1.0f, 0.0f) : (float3)(1.0f, 0.0f, 0.0f);
		float3 u = normalize(cross(axis, w));
		float3 v = cross(w, u);

		/* use the coordinte frame and random numbers to compute the next ray direction */
		float3 newdir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

		/* add a very small offset to the hitpoint to prevent self intersection */
		ray.origin = hitpoint + normal_facing * EPSILON;
		ray.dir = newdir;

		/* add the colour and light contributions to the accumulated colour */
		accum_color += mask * hitobject.emi; 

		/* the mask colour picks up surface colours at each bounce */
		mask *= hitobject.color; 
		
		/* perform cosine-weighted importance sampling for diffuse surfaces*/
		mask *= dot(newdir, normal_facing);
	}
	return accum_color;
}

float3 get_direct_light(__constant t_object *objects, const t_ray *camray, const int sphere_count,
						__constant t_light *lights, const int num_lights)
{
	t_ray ray = *camray;

	float3 bg_color = (float3)(0.3f, 0.2f, 0.5f);
	float3 hit_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 diffuse = (float3)(0.0f, 0.0f, 0.0f);
	float3 specular = (float3)(0.0f, 0.0f, 0.0f);

	float3 normal = (float3)(0.0f, 0.0f, 0.0f);
	float3 dummy_normal;

	float t = 1e20f;   /* distance to intersection */
	int hitobject_id = 0; /* index of intersected sphere */

	/* if ray misses scene, return background colour */
	if (!intersect_scene(objects, sphere_count, &ray, &normal, &t, &hitobject_id))
		return bg_color;

	/* else, we've got a hit! Fetch the closest hit sphere */
	t_object hitobject = objects[hitobject_id]; /* version with local copy of sphere */

	/* compute the hitpoint using the ray equation */
	float3 hitpoint = hitpoint_calc(&ray, t);
	
	/* compute the surface normal and flip it if necessary to face the incoming ray */
	// normal = find_normal(&hitobject, &ray, t);

	/* add a very small offset to the hitpoint to prevent self intersection */
	ray.origin = hitpoint + normal * EPSILON;
	float dist_to_light = 0;

	for (int i = 0; i < num_lights; i++)
	{
		ray.dir = fast_normalize(lights[i].pos - ray.origin);
		dist_to_light = fast_length(lights[i].pos - ray.origin);

		if (intersect_scene(objects, sphere_count, &ray, &dummy_normal, &t, &hitobject_id) && t < dist_to_light)
			continue;

		float intensity = zero_clamp(dot(normal, ray.dir));
		diffuse += intensity * lights[i].emission * hitobject.color;

		float3 refl_dir = fast_normalize((2 * intensity * normal) - ray.dir);

		float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * camray->dir));
		if (intensity == 0.0f)
			phong_term = 0.0f;

		specular += lights[i].emission * pow(phong_term, hitobject.specular_exp);

		// reflection
		// t_ray refl_ray;
		// refl_ray.origin = ray.origin;
		// refl_ray.dir = refl_dir;

		// if (intersect_scene(objects, sphere_count, &refl_ray, &t, &hitobject_id))
		// 	hit_color += objects[hitobject_id].color * 0.4f;
	}

 	hit_color = hitobject.diffuse * diffuse + hitobject.specular * specular;

	return hit_color;
}

__kernel void render_scene(__constant t_object *objects, const int sphere_count,
					__constant t_light *lights, const int num_lights,
					__global float3 *output, const int width, const int height,
					__constant t_camera *camera)
{
	/* the unique global id of the work item for the current pixel */
	unsigned int work_item_id = get_global_id(0);
	/* x-coordinate of the pixel */
	unsigned int x_coord = work_item_id % width;
	/* y-coordinate of the pixel */
	unsigned int y_coord = work_item_id / width;
	
	/* seeds for random number generator */
	// unsigned int seed0 = x_coord;
	// unsigned int seed1 = y_coord;

	t_ray camray = create_cam_ray(camera, x_coord, y_coord, width, height);

	/* add the light contribution of each sample and average over all samples*/
	// float invSamples = 1.0f / SAMPLES;

	float3 finalcolor = get_direct_light(objects, &camray, sphere_count, lights, num_lights);

	// ----------- PATH TRACING! -----------
	// for (int i = 0; i < SAMPLES; i++)
	// 	finalcolor += trace_path(spheres, &camray, sphere_count, &seed0, &seed1) * invSamples;

	/* store the pixelcolour in the output buffer */
	output[work_item_id] = finalcolor;
}
