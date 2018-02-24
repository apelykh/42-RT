/* required to compensate for limited float precision */
__constant float EPSILON = 0.00003f;
__constant float PI = 3.14159265359f;
__constant int SAMPLES = 500;
__constant int NUM_BOUNCES = 9;


typedef struct	s_ray
{
	float3 origin;
	float3 dir;
}				t_ray;

typedef struct	s_sphere
{
	float radius;
	float3 pos;
	float3 color;
	float3 emission;
}				t_sphere;

typedef struct	s_light
{
	float3 pos;
	float3 emission;
}				t_light;
// -----------------------------------------------------------------------------------------------------------------

static float get_random(unsigned int *seed0, unsigned int *seed1);
t_ray create_cam_ray(const int x_coord, const int y_coord, const int width, const int height);
float intersect_sphere(const t_sphere *sphere, const t_ray *ray);
bool intersect_scene(__constant t_sphere *spheres, const t_ray *ray, float *t, int *sphere_id, const int sphere_count);
float3 apply_direct_light(__constant t_sphere *spheres, const int sphere_count, const t_ray *camray, __constant t_light *light);
float3 trace_path(__constant t_sphere *spheres, const t_ray *camray, const int sphere_count, const int *seed0, const int *seed1);
float3 get_direct_light(__constant t_sphere *spheres, const t_ray *camray, const int sphere_count, __constant t_light *light);


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

t_ray create_cam_ray(const int x_coord, const int y_coord, const int width, const int height)
{
	/* convert int in range [0 - width] to float in range [0-1] */
	float fx = (float)x_coord / (float)width;
	/* convert int in range [0 - height] to float in range [0-1] */
	float fy = (float)y_coord / (float)height;

	/* calculate aspect ratio */
	float aspect_ratio = (float)(width) / (float)(height);
	float fx2 = (fx - 0.5f) * aspect_ratio;
	float fy2 = fy - 0.5f;

	/* determine position of pixel on screen */
	float3 pixel_pos = (float3)(fx2, -fy2, 0.0f);

	/* create camera ray*/
	t_ray ray;
	/* fixed camera position */
	ray.origin = (float3)(0.0f, 0.1f, 2.0f);
	/* vector from camera to pixel on screen */
	ray.dir = normalize(pixel_pos - ray.origin);

	return ray;
}

				/* (__global t_sphere* sphere, const t_ray* ray) */
float intersect_sphere(const t_sphere *sphere, const t_ray *ray) /* version using local copy of sphere */
{
	float3 rayToCenter = sphere->pos - ray->origin;
	float b = dot(rayToCenter, ray->dir);
	float c = dot(rayToCenter, rayToCenter) - sphere->radius*sphere->radius;
	float disc = b * b - c;

	if (disc < 0.0f) return 0.0f;
	else disc = sqrt(disc);

	if ((b - disc) > EPSILON) return b - disc;
	if ((b + disc) > EPSILON) return b + disc;

	return 0.0f;
}

bool intersect_scene(__constant t_sphere *spheres, const t_ray *ray, float *t, int *sphere_id, const int sphere_count)
{
	/* initialise t to a very large number, 
	so t will be guaranteed to be smaller
	when a hit with the scene occurs */
	float inf = 1e20f;
	*t = inf;

	/* check if the ray intersects each sphere in the scene */
	for (int i = 0; i < sphere_count; i++)
	{
		/* create local copy of sphere */
		t_sphere sphere = spheres[i];
		
		/* float hitdistance = intersect_sphere(&spheres[i], ray); */
		float hitdistance = intersect_sphere(&sphere, ray);
		/* keep track of the closest intersection and hitobject found so far */
		if (hitdistance != 0.0f && hitdistance < *t)
		{
			*t = hitdistance;
			*sphere_id = i;
		}
	}
	return *t < inf; /* true when ray interesects the scene */
}


/* the path tracing function */
/* computes a path (starting from the camera) with a defined number of bounces, accumulates light/color at each bounce */
/* each ray hitting a surface will be reflected in a random direction (by randomly sampling the hemisphere above the hitpoint) */
/* small optimisation: diffuse ray directions are calculated using cosine weighted importance sampling */

float3 trace_path(__constant t_sphere *spheres, const t_ray *camray, const int sphere_count, const int *seed0, const int *seed1)
{
	t_ray ray = *camray;

	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 mask = (float3)(1.0f, 1.0f, 1.0f);

	for (int bounces = 0; bounces < NUM_BOUNCES; bounces++)
	{
		float t;   /* distance to intersection */
		int hitsphere_id = 0; /* index of intersected sphere */

		/* if ray misses scene, return background colour */
		if (!intersect_scene(spheres, &ray, &t, &hitsphere_id, sphere_count))
			return accum_color += mask * (float3)(0.15f, 0.15f, 0.25f);

		/* else, we've got a hit! Fetch the closest hit sphere */
		t_sphere hitsphere = spheres[hitsphere_id]; /* version with local copy of sphere */

		/* compute the hitpoint using the ray equation */
		float3 hitpoint = ray.origin + ray.dir * t;
		
		/* compute the surface normal and flip it if necessary to face the incoming ray */
		float3 normal = normalize(hitpoint - hitsphere.pos); 
		float3 normal_facing = dot(normal, ray.dir) < 0.0f ? normal : normal * (-1.0f);

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
		accum_color += mask * hitsphere.emission; 

		/* the mask colour picks up surface colours at each bounce */
		mask *= hitsphere.color; 
		
		/* perform cosine-weighted importance sampling for diffuse surfaces*/
		mask *= dot(newdir, normal_facing);
	}
	return accum_color;
}

float3 get_direct_light(__constant t_sphere *spheres, const t_ray *camray, const int sphere_count, __constant t_light *light)
{
	t_ray ray = *camray;

	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);

	float t = 1e20f;   /* distance to intersection */
	int hitsphere_id = 0; /* index of intersected sphere */

	/* if ray misses scene, return background colour */
	if (!intersect_scene(spheres, &ray, &t, &hitsphere_id, sphere_count))
		return (float3)(0.0f, 0.0f, 0.0f);

	/* else, we've got a hit! Fetch the closest hit sphere */
	t_sphere hitsphere = spheres[hitsphere_id]; /* version with local copy of sphere */
	t_light local_light = *light;

	/* compute the hitpoint using the ray equation */
	float3 hitpoint = ray.origin + ray.dir * t;
	
	/* compute the surface normal and flip it if necessary to face the incoming ray */
	float3 normal = normalize(hitpoint - hitsphere.pos); 
	float3 normal_facing = dot(normal, ray.dir) < 0.0f ? normal : normal * (-1.0f);
	// float3 normal_facing = normal;

	/* add a very small offset to the hitpoint to prevent self intersection */
	ray.origin = hitpoint + normal_facing * EPSILON;
	// ray.dir = normalize(spheres[7].pos - ray.origin);
	ray.dir = normalize(local_light.pos - ray.origin);

	// float dist_to_light = fast_length(spheres[7].pos - ray.origin);
	float dist_to_light = sqrt(dot(local_light.pos - ray.origin, local_light.pos - ray.origin));

	if (!intersect_scene(spheres, &ray, &t, &hitsphere_id, sphere_count) || t > dist_to_light)
	{
		float cosine_factor = dot(normal, ray.dir);
		if (cosine_factor < 0)
	 		cosine_factor = 0;

	 	accum_color = cosine_factor * local_light.emission * hitsphere.color;

		return accum_color;
	}
 	return (float3)(0.0f, 0.0f, 0.0f);
}

__kernel void render_kernel(__constant t_sphere *spheres, const int sphere_count,
							__constant t_light *lights, const int num_lights,
							__global float3 *output, const int width, const int height)
{
	/* the unique global id of the work item for the current pixel */
	unsigned int work_item_id = get_global_id(0);
	/* x-coordinate of the pixel */
	unsigned int x_coord = work_item_id % width;
	/* y-coordinate of the pixel */
	unsigned int y_coord = work_item_id / width;
	
	/* seeds for random number generator */
	unsigned int seed0 = x_coord;
	unsigned int seed1 = y_coord;

	t_ray camray = create_cam_ray(x_coord, y_coord, width, height);

	/* add the light contribution of each sample and average over all samples*/
	float3 finalcolor = (float3)(0.0f, 0.0f, 0.0f);
	float invSamples = 1.0f / SAMPLES;

	// for (int i = 0; i < num_lights; i++)
	// {
	// 	finalcolor += apply_direct_light(spheres, sphere_count, &camray, &lights[i]);
	// }

	finalcolor = get_direct_light(spheres, &camray, sphere_count, &lights[0]);	

	// ----------- PATH TRACING! -----------
	for (int i = 0; i < SAMPLES; i++)
		finalcolor += trace_path(spheres, &camray, sphere_count, &seed0, &seed1) * invSamples;

	/* store the pixelcolour in the output buffer */
	output[work_item_id] = finalcolor;
}
