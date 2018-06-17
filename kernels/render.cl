/* required to compensate for limited float precision */
__constant float	EPSILON = 0.005;
__constant float	PI = 3.14159265359f;
__constant int		NUM_BOUNCES = 4;
__constant float3	BG_COLOR = (float3)(0.1f, 0.1f, 0.1f);


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
# define BOCAL			9

# define AMBIENT		0
# define POINT			1
# define PARALLEL		2
# define SPOT			3

typedef struct	s_ray
{
	float3		origin;
	float3		dir;
}				t_ray;

typedef float16 t_mat4;

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
	int			id;					// object's ID corresponds to it's order in array of objects
	bool		hidden;				// if true - object won't be rendered unless called by some boolean operation
	int			type;				// primitive type or operation type
	int2		operand;			// ID's of 2 objects upon which to perform operation. Both can be primitives or other operations
	bool		capped;				// defines whether cylinder or cone are capped or unlimited
	float3		location;			// object is moved by this vector when transformed into world coordinates
	float3		rotation;			// object is rotated by .x degrees along X axis, .y degrees along Y axis, .z degrees along Z axis when transformed into world coords
	float3		scale;				// object is scaled x times along X axis ... when transfored into world coords
	float3		color;				// .x - R, .y - G, .z - B pigments of object's color
	float		transparency;
	float		spec_pow;
	float		ior;
	float		kr;
	t_mat4		from_local;			// 4x4 homogenous matrix of transformation from object's local space to world space
	t_mat4		to_local;			// 4x4 homogenous matrix of transformation to object's local space from world space
}				t_object;

typedef struct	s_light
{
	int			type;
	float		angle;
	float3		dir;
	float3		location;
	float3		emission;
}				t_light;

typedef struct	s_camera
{
	float3		location;
	float3		rotation;
	float		fov;
	t_mat4		translate_matrix;
	t_mat4		rotation_matrix;
}				t_camera;

/* ----- primitive_intersections.cl ----- */
bool	intersect_sphere(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_plane(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_cylinder(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_cone(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_box(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);

/* ----- compound_intersections.cl ----- */
bool	intersect_union(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool 	intersect_difference(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool	intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);
bool 	intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, t_hitpoints *hit);

// bool	intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray, t_point *hitpoint);
bool	intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray,
					t_point *hit, t_point *next_hit);

/* ----- matrix_utils.cl ----- */
t_mat4	mat_translate(float3 v);
t_mat4	mat_scale(float3 s);
t_mat4	mat_rotatex(float angle);
t_mat4	mat_rotatey(float angle);
t_mat4	mat_rotatez(float angle);
t_mat4	mat_rotatea(float angle, float3 axis);
t_mat4	mat_invert(t_mat4 m);
float4	mat_mult_vec(t_mat4 m, float4 v);
t_mat4	mat_transpose(t_mat4 m);
t_mat4	mat_mult_mat(t_mat4 a, t_mat4 b);
t_mat4	mat_transform(const t_object *obj);

// static float get_random(unsigned int *seed0, unsigned int *seed1);
static float zero_clamp(float x);
t_ray create_cam_ray(__constant t_camera *camera, const int x_coord, const int y_coord, const int width, const int height);
// float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int object_count, const int *seed0, const int *seed1);
float3 add_sepia(float3 color);
float3 get_direct_light(__constant t_object *spheres, const t_ray *camray, const int object_count, __constant t_light *lights, const int num_lights);

float3	shade(float3 incident_dir, t_point *hit, __constant t_object *objects, const int num_objects,
										__constant t_light *lights, const int num_lights);

float	fresnel_reflect_amount(float n1, float n2, float3 normal, float3 incident, float obj_kr);

void	hitpoints_init(t_hitpoints* hit);
void	hitpoints_sort(t_hitpoints* hit);
t_ray	ray2local(const t_ray *r, __constant t_object *o);


#include "kernels/primitive_intersections.cl"
#include "kernels/compound_intersections.cl"
#include "kernels/matrix_utils.cl" 

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

	float3 pixel = (float3)(px, py, -1.f);

	t_ray ray;
	ray.origin = (float3)(0.f, 0.f, 0.f);
	ray.dir = normalize(pixel);
	t_mat4 transform_to_world = mat_mult_mat(camera->translate_matrix, camera->rotation_matrix);
	
	ray.origin = mat_mult_vec(transform_to_world, (float4)(ray.origin, 1.0f)).xyz;
	ray.dir = fast_normalize(mat_mult_vec(transform_to_world, (float4)(ray.dir, 0.0f)).xyz);

	return ray;
}

bool intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray,
					t_point *hit, t_point *next_hit)
{
	int	hit_occured = 0;

	t_hitpoints hitpoints;
	hitpoints_init(&hitpoints);

	for (int i = 0; i < num_objects; i++) 
	{
		if (!objects[i].hidden)
		{
			switch(objects[i].type)
			{
			case PLANE :
				if (intersect_plane(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case SPHERE :
				if (intersect_sphere(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case CYLINDER :
				if (intersect_cylinder(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case CONE :
				if(intersect_cone(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case BOX :
				if(intersect_box(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case UNION :
				if(intersect_union(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case DIFFERENCE :
				if (intersect_difference(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case INTERSECTION :
				if (intersect_intersection(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case CLIPPING :
				if (intersect_clipping(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			case BOCAL :
				if (intersect_bocal(objects, i, ray, &hitpoints))
					hit_occured++;
				break;
			}
		}
	}
	if (hit_occured == 0)
		return false;

	hitpoints_sort(&hitpoints);
	*hit = hitpoints.pt[0];

	if (hitpoints.pt[0].obj_id == -1 || hitpoints.pt[0].dist <= 0.0f)
		return false;

	if (next_hit)
	{
		int j = 1;
		while (hitpoints.pt[j].obj_id == hitpoints.pt[0].obj_id &&
				hitpoints.pt[j].obj_id > 0)
			j++;

		*next_hit = hitpoints.pt[j];
	}

	return true;
}

float3	shade(float3 incident_dir, t_point *hit, __constant t_object *objects, const int num_objects,
										__constant t_light *lights, const int num_lights)
{
	float3	diffuse = (float3)(0.0f, 0.0f, 0.0f);
	float3	specular = (float3)(0.0f, 0.0f, 0.0f);
	float3	normal = (float3)(0.0f, 0.0f, 0.0f);
	t_point hit_tmp;

	t_ray	sec_ray;

	t_object hitobject = objects[hit->obj_id];
	normal = hit->normal;
	sec_ray.origin = hit->pos + normal * EPSILON;

	float dist_to_light = 1e20f;
	for (int i = 0; i < num_lights; i++)
	{
		if (lights[i].type == AMBIENT)
		{
			diffuse += lights[i].emission * hitobject.color;
			continue;
		}
		if (lights[i].type == PARALLEL)
			sec_ray.dir = -1.0f * fast_normalize(lights[i].dir);
		else
		{
			sec_ray.dir = fast_normalize(lights[i].location - sec_ray.origin);
			dist_to_light = fast_length(lights[i].location - sec_ray.origin);
		}

		if (intersect_scene(objects, num_objects, &sec_ray, &hit_tmp, 0) &&
			hit_tmp.dist < dist_to_light)
			continue;

		if (lights[i].type == SPOT)
		{
			float k = acos(dot((-1.0f) * sec_ray.dir, fast_normalize(lights[i].dir)));
			if (degrees(k) >= (lights[i].angle / 2.0f))
				continue;
		}

		float intensity = zero_clamp(dot(normal, sec_ray.dir));
		diffuse += intensity * lights[i].emission * hitobject.color * (1 - hitobject.kr);

		if (hitobject.spec_pow > 0.0)
		{
			float3 refl_dir = fast_normalize((2 * intensity * normal) - sec_ray.dir);
			// float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * ray->dir));
			float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * incident_dir));
			specular += lights[i].emission * pow(phong_term, hitobject.spec_pow);
		}
	}
 	return (diffuse + specular);
}

float	fresnel_reflect_amount(float n1, float n2, float3 normal, float3 incident, float obj_kr)
{
    // Schlick aproximation
    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    float cosX = -dot(normal, incident);
    if (n1 > n2)
    {
        float n = n1 / n2;
        float sinT2 = n * n * (1.0f - cosX * cosX);
        // Total internal reflection
        if (sinT2 > 1.0f)
            return (1.0f);

        cosX = sqrt(1.0 - sinT2);
    }
    float x = 1.0f - cosX;
    float ret = r0 + (1.0f - r0) * x * x * x * x * x;

    // adjust reflect multiplier for object reflectivity
    ret = (obj_kr + (1.0f - obj_kr) * ret);

    return (ret);
}

static float3 refract(float n1, float n2, float3 normal, float3 incident)
{
	float3 refr_dir = (float3)(0.0f, 0.0f, 0.0f);

	float eta = n1 / n2;

	float dotprod = dot(incident, normal);
	float3 a = eta * (incident - normal * dotprod);
	float sinT2 = eta * eta * (1 - dotprod * dotprod);

	// Case of total internal reflection, no refracted ray!
	// returned direction is (float3)(0.0f, 0.0f, 0.0f)
	if (n1 > n2 && sinT2 > 1.0f)
		return (refr_dir);

	float3 b = normal * sqrt(1 - sinT2);
	refr_dir = a - b;

	return (refr_dir);
}

static float3 get_reflected_color(t_point hit, t_ray ray, float fresnel,
	__constant t_object *objects, const int num_objects, __constant t_light *lights, const int num_lights)
{
	t_ray	refl_ray;
	t_point	next_hit;
	float3	refl_color = (float3)(0.0f, 0.0f, 0.0f);

	float decay_rate = fresnel;

	for (int bounces = 0; bounces < NUM_BOUNCES; bounces++)
	{
		// if (fresnel > EPSILON)
		if (objects[hit.obj_id].kr > 0.0f)
		{
			float c1 = (-1.0f) * dot(hit.normal, ray.dir);
			refl_ray.origin = hit.pos + hit.normal * EPSILON;
			refl_ray.dir = fast_normalize(ray.dir + 2 * hit.normal * c1);

			if (!intersect_scene(objects, num_objects, &refl_ray, &hit, &next_hit))
			{
				refl_color += decay_rate * BG_COLOR;
				break;
			}
			fresnel = fresnel_reflect_amount(1.0f, 1.0f, refl_ray.dir, hit.normal, objects[hit.obj_id].kr);

			if (objects[hit.obj_id].kr < 1.0f)
			{
				refl_color += decay_rate * (1 - objects[hit.obj_id].transparency) *
					shade(refl_ray.dir, &hit, objects, num_objects, lights, num_lights);

				// refl_color += objects[hit.obj_id].transparency *
				// 	get_transparency(refl_ray, hit, next_hit, objects, num_objects, lights, num_lights);

				// refl_color += objects[next_hit.obj_id].transparency *
				// 	shade(refl_ray.dir, &next_hit, objects, num_objects, lights, num_lights);
			}
			// if (objects[next_hit.obj_id].transparency > EPSILON)
			// 	refl_color += decay_rate * (1 - fresnel) * objects[next_hit.obj_id].color;

			decay_rate *= fresnel;
			ray = refl_ray;
		}
		else
			break;
	}

	return (refl_color);
}


static float3 get_transparency(t_ray incident, t_point first_hit, float fresnel, t_point next_hit,
	__constant t_object *objects, const int num_objects, __constant t_light *lights, const int num_lights)
{
	t_ray	refr_ray = incident;
	float3	refr_color = (float3)(0.0f, 0.0f, 0.0f);

	refr_ray.origin = first_hit.pos + first_hit.normal * EPSILON;

	// if there is a hit with the next object after primary
	if (next_hit.obj_id != -1)
	{
		refr_color += (1 - fresnel) * shade(incident.dir, &next_hit, objects, num_objects, lights, num_lights);

		float next_fresnel = fresnel_reflect_amount(1.0f, 1.0f, refr_ray.dir,
			next_hit.normal, objects[next_hit.obj_id].kr);

		refr_color += next_fresnel *
			get_reflected_color(next_hit, refr_ray, next_fresnel, objects, num_objects, lights, num_lights);
	}
	// ray hits background after primary object
	else
		refr_color += (1 - fresnel) * BG_COLOR;

	return (refr_color);
}


float3 get_direct_light(__constant t_object *objects, const t_ray *camray, const int num_objects,
						__constant t_light *lights, const int num_lights)
{
	t_ray	ray = *camray;
	t_point hit;
	t_point first_hit;
	t_point next_hit;

	if (!intersect_scene(objects, num_objects, &ray, &first_hit, &next_hit))
		return BG_COLOR;

	hit = first_hit;

	float fresnel = fresnel_reflect_amount(1.0f, 1.0f, ray.dir, hit.normal, objects[hit.obj_id].kr);

	float3 diffuse_color =
		(1 - objects[hit.obj_id].transparency) * (1 - fresnel) *
		shade(ray.dir, &hit, objects, num_objects, lights, num_lights);

	// TRANSPARENCY
	float3 refr_color = (float3)(0.0f, 0.0f, 0.0f);
	if (fresnel < (1.0f - EPSILON) && objects[first_hit.obj_id].transparency > 0.0f)
	{
		refr_color = 0.5f * 
			objects[first_hit.obj_id].transparency * (1 - fresnel) * 
			get_transparency(*camray, first_hit, fresnel, next_hit, objects, num_objects, lights, num_lights);
	}

	// REFLECTION
	float3 refl_color = (float3)(0.0f, 0.0f, 0.0f);
	if (fresnel > EPSILON)
		refl_color = fresnel * get_reflected_color(hit, ray, fresnel, objects, num_objects, lights, num_lights);

	return (diffuse_color + refl_color + refr_color);
}

float3 add_sepia(float3 color)
{
	float3 sepia;

	sepia.x = 0.393 * color.x + 0.769 * color.y + 0.189 * color.z;
 	sepia.y = 0.349 * color.x + 0.686 * color.y + 0.168 * color.z;
 	sepia.z = 0.272 * color.x + 0.534 * color.y + 0.131 * color.z;
 	sepia.x = (sepia.x > 1.0f) ? 1.0f : sepia.x;
 	sepia.y = (sepia.y > 1.0f) ? 1.0f : sepia.y;
 	sepia.z = (sepia.z > 1.0f) ? 1.0f : sepia.z;
 	return (sepia);
}

__kernel void render_scene(__constant t_object *objects, const int object_count,
					__constant t_light *lights, const int num_lights,
					__global uchar4 *output, const int width, const int height,
					__constant t_camera *camera, const int sepia)
{
	unsigned int work_item_id = get_global_id(0);
	unsigned int x_coord = work_item_id % width;
	unsigned int y_coord = work_item_id / width;

	t_ray camray = create_cam_ray(camera, x_coord, y_coord, width, height);

	float3 pixel_float = get_direct_light(objects, &camray, object_count, lights, num_lights);

	if (sepia)
		pixel_float = add_sepia(pixel_float);

	output[work_item_id].z = (uchar)(clamp(pixel_float.x, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].y = (uchar)(clamp(pixel_float.y, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].x = (uchar)(clamp(pixel_float.z, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].w = 1;
}
