/* required to compensate for limited float precision */
// __constant float EPSILON = 0.00003f;
__constant float EPSILON = 0.005f;
__constant float PI = 3.14159265359f;
// __constant int SAMPLES = 500;
// __constant int NUM_BOUNCES = 7;


# define MAX_POINTS		32

# define PLANE			0
# define SPHERE			1
# define CYLINDER		2
# define CONE			3
# define BOX			4
# define UNION			5
# define INTERSECTION	6
# define DIFFERENCE		7
# define CLIPPING		8

// # define AMBIENT		0
// # define POINT			1
// # define PARALLEL		2
// # define SPOT			3

typedef struct	s_ray
{
	float3		origin;
	float3		dir;
}				t_ray;

typedef float16 mat4;

typedef struct	s_point
{
	int			obj_id;				// ID of object which we intersected. -1 if we need to 'delete' this point at next sorting
	float		dist;				// distance travelled by ray before intersection
	float3		pos;				// position of intersection point in space
	float3		normal;				// normal vector
	bool		inside;				// true - ray enters volume of object at intersection, false - ray exits volume of object
}				t_point;

typedef struct	s_hitpoints
{
	t_point		pt[MAX_POINTS];		// MAX_POINTS is defined, maximum amount of points of intersection per 1 ray
	int			num_elements;			// counter for tracking number of points stored in array. when sorting we don't check all points, but only valid ones (id != -1, dist > 0.0)
}				t_hitpoints;


// all objects (primitive or compound) are defined in their local space, they rotate and scale around origin point
typedef struct	s_object
{
	int		id;					// object's ID corresponds to it's order in array of objects
	bool	hidden;				// if true - object won't be rendered unless called by some boolean operation
	int		type;				// primitive type or operation type
	int2	operand;			// ID's of 2 objects upon which to perform operation. Both can be primitives or other operations
	bool	capped;				// defines whether cylinder or cone are capped or unlimited
	float3	location;			// object is moved by this vector when transformed into world coordinates
	float3	rotation;			// object is rotated by .x degrees along X axis, .y degrees along Y axis, .z degrees along Z axis when transformed into world coords
	float3	scale;				// object is scaled x times along X axis ... when transfored into world coords
	float3	color;				// .x - R, .y - G, .z - B pigments of object's color
	float	diffuse;
	float	specular;
	float	specular_exp;
	mat4 from_local;			// 4x4 homogenous matrix of transformation from object's local space to world space
	mat4 to_local;				// 4x4 homogenous matrix of transformation to object's local space from world space
}				t_object;

typedef struct	s_light
{
	// int		type;
	float3	location;
	float3	emission;
}				t_light;

typedef struct	s_camera
{
	float3		location;
	float3		rotation;
	float		fov;
	mat4		translate_matrix;
	mat4		rotation_matrix;
}				t_camera;

/* ----- primitive_intersections.cl ----- */
bool	intersect_sphere(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_plane(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_cylinder(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_cone(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_box(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);

/* ----- compound_intersections.cl ----- */
bool	intersect_union(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool 	intersect_difference(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool 	intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);

// bool	intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray, float3 *normal,
// 							float *t, int *object_id, bool quick);
bool intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray,
						t_point *hitpoint, bool quick);

/* ----- matrix_utils.cl ----- */
mat4	mat_translate(float3 v);
mat4	mat_scale(float3 s);
mat4	mat_rotatex(float angle);
mat4	mat_rotatey(float angle);
mat4	mat_rotatez(float angle);
mat4	mat_rotatea(float angle, float3 axis);
mat4	mat_invert(mat4 m);
float4	mat_mult_vec(mat4 m, float4 v);
mat4	mat_transpose(mat4 m);
mat4	mat_mult_mat(mat4 a, mat4 b);
mat4	mat_transform(const t_object *obj);

// static float get_random(unsigned int *seed0, unsigned int *seed1);
static float zero_clamp(float x);
t_ray create_cam_ray(__constant t_camera *camera, const int x_coord, const int y_coord, const int width, const int height);
float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int object_count, const int *seed0, const int *seed1);
float3 get_direct_light(__constant t_object *spheres, const t_ray *camray, const int object_count, __constant t_light *lights, const int num_lights);

float3 trace(t_ray *ray, t_point *hit, __constant t_object *objects, const int num_objects,
									   __constant t_light *lights, const int num_lights);

void	hitpoints_init(t_hitpoints* hit);
void	hitpoints_sort(t_hitpoints* hit);
t_ray	ray2local(const t_ray *r, __constant t_object *o);


#include "kernels/primitive_intersections.cl"
#include "kernels/compound_intersections.cl"
#include "kernels/matrix_utils.cl" 

__constant float3 bg_color = (float3)(0.15f, 0.15f, 0.15f);

static float zero_clamp(float x)
{
	return (x < 0.0f) ? 0.0f : x;
}

t_ray ray2local(const t_ray *r, __constant t_object *o)
{
	t_ray ray;

	ray.dir = fast_normalize(mat_mult_vec(o->to_local, (float4)(r->dir, 0.0f)).xyz);
	ray.origin = mat_mult_vec(o->to_local, (float4)(r->origin, 1.0f)).xyz;
	return ray;
}

void hitpoints_init(t_hitpoints* hit)
{
	hit->num_elements = 0;
	for (int i = 0; i < MAX_POINTS; i++)
	{
		hit->pt[i].obj_id = -1;
	}
}

void hitpoints_sort(t_hitpoints* hit)
{
	t_point	tmp;
	int		to_del = 0;

	for(int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == -1)
		{
			hit->pt[i].dist = 0.0f;
			++to_del;
		}
	}
	for(int i = 0; i < hit->num_elements - 1; i++)
	{
		for(int j = 0; j < hit->num_elements - i - 1; j++)
		{
			if (hit->pt[j + 1].dist > 0.0f &&
				(hit->pt[j + 1].dist < hit->pt[j].dist || hit->pt[j].dist <= 0.0f))
			{
				tmp = hit->pt[j];
				hit->pt[j] = hit->pt[j + 1];
				hit->pt[j + 1] = tmp;
			}
		}
	}
	hit->num_elements -= to_del;
	hit->num_elements = max(hit->num_elements, 0);
}

t_ray create_cam_ray(__constant t_camera *camera, const int x, const int y,
						const int width, const int height)
{
	float aspect_ratio = (float)(width) / (float)(height);
	float tan_half_fov = tan(radians(camera->fov / 2));

	float px = (2 * ((x + 0.5) / width) - 1) * 1.0f * aspect_ratio * tan_half_fov;
	float py = (1 - 2 * ((y + 0.5) / height) * 1.0f) * tan_half_fov;

	// float3 origin = (float3)(0.0f, 0.0f, 0.0f);

	// mat4 cam2world = mat_translate(camera->loc);

	// float3 origin_world, pixel_pos_world;
	// origin_world = mat_mult_vec(cam2world, (float4)(origin, 1.0f)).xyz;
	// pixel_pos_world = mat_mult_vec(cam2world, (float4)(px, py, -1, 1.0f)).xyz;
	float3 pixel = (float3)(px, py, -1.f);

	t_ray ray;
	ray.origin = (float3)(0.f, 0.f, 0.f);
	ray.dir = normalize(pixel);
	mat4 transform_to_world = mat_mult_mat(camera->translate_matrix, camera->rotation_matrix);
	
	ray.origin = mat_mult_vec(transform_to_world, (float4)(ray.origin, 1.0f)).xyz;
	ray.dir = fast_normalize(mat_mult_vec(transform_to_world, (float4)(ray.dir, 0.0f)).xyz);

	return ray;
}

bool intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray, t_point *hitpoint, bool quick)
{
	int	hit_occured = 0;

	t_hitpoints hit;
	hitpoints_init(&hit);

	for (int i = 0; i < num_objects; i++) 
	{
		if (!objects[i].hidden)
		{
			switch(objects[i].type)
			{
			case PLANE :
				if (intersect_plane(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case SPHERE :
				if (intersect_sphere(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case CYLINDER :
				if (intersect_cylinder(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case CONE :
				if(intersect_cone(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case BOX :
				if(intersect_box(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case UNION :
				if(intersect_union(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case DIFFERENCE :
				if (intersect_difference(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case INTERSECTION :
				if (intersect_intersection(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			case CLIPPING :
				if (intersect_clipping(objects, i, ray, quick, &hit))
					hit_occured++;
				break;
			}
		}
	}
	if (hit_occured == 0)
		return false;

	hitpoints_sort(&hit);
	*hitpoint = hit.pt[0];

	if (hit.pt[0].obj_id == -1 || hit.pt[0].dist <= 0.0f)
		return false;

	return true;
}

float3 trace(t_ray *ray, t_point *hit, __constant t_object *objects, const int num_objects,
									   __constant t_light *lights, const int num_lights)
{
	float3	hit_color = (float3)(0.0f, 0.0f, 0.0f);
	float3	diffuse = (float3)(0.0f, 0.0f, 0.0f);
	float3	specular = (float3)(0.0f, 0.0f, 0.0f);
	float3	normal = (float3)(0.0f, 0.0f, 0.0f);
	t_point hit_tmp;

	t_ray	sec_ray;

	t_object hitobject = objects[hit->obj_id];
	normal = hit->normal;
	sec_ray.origin = hit->pos + normal * EPSILON;

	// ambient
	diffuse = hitobject.color * 0.2f;

	float dist_to_light = 0;
	for (int i = 0; i < num_lights; i++)
	{
		sec_ray.dir = fast_normalize(lights[i].location - sec_ray.origin);
		dist_to_light = fast_length(lights[i].location - sec_ray.origin);

		if (intersect_scene(objects, num_objects, &sec_ray, &hit_tmp, true) &&
			hit_tmp.dist < dist_to_light)
			continue;

		float intensity = zero_clamp(dot(normal, sec_ray.dir));
		diffuse += intensity * 0.8f * lights[i].emission * hitobject.color;

		float3 refl_dir = fast_normalize((2 * intensity * normal) - sec_ray.dir);

		float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * ray->dir));
		if (intensity == 0.0f)
			phong_term = 0.0f;

		specular += lights[i].emission * pow(phong_term, hitobject.specular_exp);
	}
 	hit_color = hitobject.diffuse * diffuse + hitobject.specular * specular;

 	return (hit_color);
}

// float3 trace(t_ray *ray, t_point *hit, __constant t_object *objects, const int num_objects,
// 									   __constant t_light *lights, const int num_lights)
// {
// 	float3	hit_color = (float3)(0.0f, 0.0f, 0.0f);
// 	float3	diffuse = (float3)(0.0f, 0.0f, 0.0f);
// 	float3	specular = (float3)(0.0f, 0.0f, 0.0f);
// 	float3	normal = (float3)(0.0f, 0.0f, 0.0f);
// 	t_point hit_tmp;

// 	t_ray	sec_ray;

// 	// t_object hitobject = objects[hit->obj_id];
// 	normal = hit->normal;
// 	sec_ray.origin = hit->pos + normal * EPSILON;

// 	// ambient
// 	diffuse = objects[hit->obj_id].color * 0.2f;

// 	float dist_to_light = 0;
// 	for (int i = 0; i < num_lights; i++)
// 	{
// 		sec_ray.dir = fast_normalize(lights[i].location - sec_ray.origin);
// 		dist_to_light = fast_length(lights[i].location - sec_ray.origin);

// 		if (intersect_scene(objects, num_objects, &sec_ray, &hit_tmp, true) &&
// 			hit_tmp.dist < dist_to_light)
// 			continue;

// 		float intensity = zero_clamp(dot(normal, sec_ray.dir));
// 		diffuse += intensity * 0.8f * lights[i].emission * objects[hit->obj_id].color;

// 		float3 refl_dir = fast_normalize((2 * intensity * normal) - sec_ray.dir);

// 		float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * ray->dir));
// 		if (intensity == 0.0f)
// 			phong_term = 0.0f;

// 		specular += lights[i].emission * pow(phong_term, objects[hit->obj_id].specular_exp);
// 	}
//  	hit_color = objects[hit->obj_id].diffuse * diffuse + objects[hit->obj_id].specular * specular;

//  	return (hit_color);
// }

float3 get_direct_light(__constant t_object *objects, const t_ray *camray, const int num_objects,
						__constant t_light *lights, const int num_lights)
{
	t_ray	ray = *camray;
	t_point hit;

	if (!intersect_scene(objects, num_objects, &ray, &hit, false))
		return bg_color;

	// reflection
	// float3 hit_color = (float3)(0.0f, 0.0f, 0.0f);
	t_ray refl_ray;
	float kr = 0.6f;
	float r = kr;
	float3 hit_color = (1 - kr) * trace(&ray, &hit, objects, num_objects, lights, num_lights);

	for (int bounces = 0; bounces < 3; bounces++)
	{
		refl_ray.origin = hit.pos + hit.normal * EPSILON;

		float c1 = (-1.0f) * dot(hit.normal, ray.dir);
		refl_ray.dir = ray.dir + 2 * hit.normal * c1;

		if (intersect_scene(objects, num_objects, &refl_ray, &hit, true))
		{
			hit_color += r * trace(&refl_ray, &hit, objects, num_objects, lights, num_lights);

			// r *= objects[hit.obj_id].kr;
			r *= kr;
		}
		ray = refl_ray;
	}

	return hit_color;
}

__kernel void render_scene(__constant t_object *objects, const int object_count,
					__constant t_light *lights, const int num_lights,
					__global uchar4 *output, const int width, const int height,
					__constant t_camera *camera)
{
	unsigned int work_item_id = get_global_id(0);
	unsigned int x_coord = work_item_id % width;
	unsigned int y_coord = work_item_id / width;

	t_ray camray = create_cam_ray(camera, x_coord, y_coord, width, height);

	float3 pixel_float = get_direct_light(objects, &camray, object_count, lights, num_lights);

	output[work_item_id].z = (uchar)(clamp(pixel_float.x, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].y = (uchar)(clamp(pixel_float.y, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].x = (uchar)(clamp(pixel_float.z, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].w = 1;
}
