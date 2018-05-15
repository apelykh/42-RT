/* required to compensate for limited float precision */
// __constant float EPSILON = 0.00003f;
__constant float EPSILON = 0.005f;
__constant float PI = 3.14159265359f;
__constant int SAMPLES = 500;
__constant int NUM_BOUNCES = 7;


# define MAX_POINTS 32

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
	float3 origin;
	float3 dir;
}				t_ray;

typedef float16 mat4;

typedef struct	s_point
{
	int		obj_id;				// ID of object which we intersected. -1 if we need to 'delete' this point at next sorting
	float	dist;				// distance travelled by ray before intersection
	float3	pos;				// position of intersection point in space
	float3	normal;				// normal vector
	bool	inside;				// true - ray enters volume of object at intersection, false - ray exits volume of object

}				t_point;

typedef struct	s_hitpoints
{
	t_point	pt[MAX_POINTS];		// MAX_POINTS is defined, maximum amount of points of intersection per 1 ray
	int num_elements;			// counter for tracking number of points stored in array. when sorting we don't check all points, but only valid ones (id != -1, dist > 0.0)
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
	// float3	emi;				// object's emission
	float	diffuse;
	float	specular;
	float	specular_exp;
	mat4 from_local;			// 4x4 homogenous matrix of transformation from object's local space to world space
	mat4 to_local;				// 4x4 homogenous matrix of transformation to object's local space from world space
	// char	rot_change;
	// char	tra_change;
	// char	sca_change;
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


static float get_random(unsigned int *seed0, unsigned int *seed1);
static float zero_clamp(float x);
t_ray create_cam_ray(__constant t_camera *camera, const int x_coord, const int y_coord, const int width, const int height);
float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int object_count, const int *seed0, const int *seed1);
float3 get_direct_light(__constant t_object *spheres, const t_ray *camray, const int object_count, __constant t_light *lights, const int num_lights);

// -------------------------- Object intersections ----------------------------
bool	intersect_sphere(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_plane(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_cylinder(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_cone(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_box(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);

bool	intersect_union(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool 	intersect_difference(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool	intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);
bool 	intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit);

bool	intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray, float3 *normal,
							float *t, int *object_id, bool quick);

float3 get_diffuse_light(__constant t_object *objects, const int num_objects, float3 hitpoint, int h_id, float3 normal,
						 __constant t_light *lights, const int num_lights);


float3	hitpoint_calc(const t_ray *ray, float dist);
mat4	mat_transform(const t_object *obj);
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
void	hitpoints_init(t_hitpoints* hit);
void	hitpoints_sort(t_hitpoints* hit);
t_ray	ray2local(const t_ray *r, __constant t_object *o);

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
		hit->pt[i].obj_id = -1;
}

void hitpoints_sort(t_hitpoints* hit)
{
	int i = 0;
	t_point tmp;
	int todelete = 0;
	while (i != hit->num_elements - 1)
	// while (i != MAX_POINTS - 1)
	{
		if (hit->pt[i].obj_id == -1)
		{
			hit->pt[i].dist = 0.0f;
			++todelete;
		}
		if (hit->pt[i + 1].dist > 0.0f && (hit->pt[i + 1].dist < hit->pt[i].dist || hit->pt[i].dist <= 0.0f))
		{
			tmp = hit->pt[i];
			hit->pt[i] = hit->pt[i + 1];
			hit->pt[i + 1] = tmp;
			i = 0;
		}
		else
			i++;
	}
	hit->num_elements -= todelete;
	hit->num_elements = max(hit->num_elements, 0);
}

//MATRIX ZONE

mat4	mat_transform(const t_object *obj)
{
	mat4 translate, rotate, scale = (float16)(0.f);
	translate = mat_translate(obj->location);
	rotate = mat_rotatex(obj->rotation.x);
	rotate = mat_mult_mat(mat_rotatey(obj->rotation.y), rotate);
	rotate = mat_mult_mat(mat_rotatez(obj->rotation.z), rotate);
	scale = mat_scale(obj->scale);
	return (mat_mult_mat(mat_mult_mat(translate, rotate), scale));
}

mat4	mat_translate(float3 v)
{
	mat4 r;

	r.s0123 = (float4)(1.0f, 0.0f, 0.0f, 0.0f);
	r.s4567 = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
	r.s89AB = (float4)(0.0f, 0.0f, 1.0f, 0.0f);
	r.sCDEF = (float4)(v, 1.0f);
	return r;
}

mat4	mat_scale(float3 s)
{
	mat4 r;

	r.s0123 = (float4)(s.x, 0.0f, 0.0f, 0.0f);
	r.s4567 = (float4)(0.0f, s.y, 0.0f, 0.0f);
	r.s89AB = (float4)(0.0f, 0.0f, s.z, 0.0f);
	r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	return r;
}

mat4 mat_rotatex(float angle)
{
	float sint = sin(radians(angle));
	float cost = cos(radians(angle));
	mat4 r;

	r.s0123 = (float4)(1.0f, 0.0f, 0.0f, 0.0f);
	r.s4567 = (float4)(0.0f, cost, sint, 0.0f);
	r.s89AB = (float4)(0.0f, -sint, cost, 0.0f);
	r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	return r;
}

mat4 mat_rotatey(float angle)
{
	float sint = sin(radians(angle));
	float cost = cos(radians(angle));
	mat4 r;

	r.s0123 = (float4)(cost, 0.0f, -sint, 0.0f);
	r.s4567 = (float4)(0.0f, 1.0f, 0.0f, 0.0f);
	r.s89AB = (float4)(sint, 0.0f, cost, 0.0f);
	r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	return r;
}

mat4 mat_rotatez(float angle)
{
	float sint = sin(radians(angle));
	float cost = cos(radians(angle));
	mat4 r;

	r.s0123 = (float4)(cost, sint, 0.0f, 0.0f);
	r.s4567 = (float4)(-sint, cost, 0.0f, 0.0f);
	r.s89AB = (float4)(0.0f, 0.0f, 1.0f, 0.0f);
	r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	return r;
}

mat4 mat_rotatea(float angle, float3 axis)
{
	mat4 r;
	float3 v = fast_normalize(axis);
	float s = sin(radians(angle));
	float c = cos(radians(angle));
	float ic = 1.0f - c;

	r.s0 = v.x * v.x * ic + c;
	r.s1 = v.y * v.x * ic - s * v.z;
	r.s2 = v.z * v.x * ic + s * v.y;
	r.s3 = 0.0f;

	r.s4 = v.x * v.y * ic + s * v.z;
	r.s5 = v.y * v.y * ic + c;
	r.s6 = v.z * v.y * ic - s * v.x;
	r.s7 = 0.0f;

	r.s8 = v.x * v.z * ic - s * v.y;
	r.s9 = v.y * v.z * ic + s * v.x;
	r.sA = v.z * v.z * ic + c;
	r.sB = 0.0f;

	r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
	return r;
}

mat4 mat_invert(mat4 m)
{
	mat4 r;

	float det = m.s0 * m.s5 * m.sA + m.s1 * m.s6 * m.s8 + m.s2 * m.s4 * m.s9 - m.s0 * m.s6 * m.s9 - m.s2 * m.s5 * m.s8 - m.s1* m.s4 * m.sA;

	if (det != 0.0f)
	{
		det = 1.f / det;
		r.s0123 = (float4)((m.s5 * m.sA - m.s9 * m.s6) * det, (m.s9 * m.s2 - m.s1 * m.sA) * det, (m.s1 * m.s6 - m.s5 * m.s2) * det, 0.0f);
		r.s4567 = (float4)((m.s8 * m.s6 - m.s4 * m.sA) * det, (m.s0 * m.sA - m.s8 * m.s2) * det, (m.s4 * m.s2 - m.s0 * m.s6) * det, 0.0f);
		r.s89AB = (float4)((m.s4 * m.s9 - m.s8 * m.s5) * det, (m.s8 * m.s1 - m.s0 * m.s9) * det, (m.s0 * m.s5 - m.s4 * m.s1) * det, 0.0f);
		r.sCDEF = (float4)(0.0f, 0.0f, 0.0f, 1.0f);

		float3 b = mat_mult_vec(r, -m.sCDEF).xyz;
		r.sCDE = b.xyz;
	}
	else
		r = m;
	return r;
}

mat4 mat_transpose(mat4 m)
{
	mat4 r;

	r.s0123 = m.s048C;
	r.s4567 = m.s159D;
	r.s89AB = m.s26AE;
	r.sCDEF	= m.s37BF;
	return r;
}

mat4 mat_mult_mat(mat4 a, mat4 b)
{
	mat4 c;

	c.s0123 = (float4)(dot(a.s048C, b.s0123), dot(a.s159D, b.s0123), dot(a.s26AE, b.s0123), dot(a.s37BF, b.s0123));
	c.s4567 = (float4)(dot(a.s048C, b.s4567), dot(a.s159D, b.s4567), dot(a.s26AE, b.s4567), dot(a.s37BF, b.s4567));
	c.s89AB = (float4)(dot(a.s048C, b.s89AB), dot(a.s159D, b.s89AB), dot(a.s26AE, b.s89AB), dot(a.s37BF, b.s89AB));
	c.sCDEF = (float4)(dot(a.s048C, b.sCDEF), dot(a.s159D, b.sCDEF), dot(a.s26AE, b.sCDEF), dot(a.s37BF, b.sCDEF));
	return c;
}

float4 mat_mult_vec(mat4 m, float4 v)
{
	return ((float4)(dot(m.s048C, v), dot(m.s159D, v), dot(m.s26AE, v), dot(m.s37BF, v)));
}

// END OF MATRIX ZONE

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


// unecessary function, need to find occurences and remove them
float3 hitpoint_calc(const t_ray *ray, float dist) 
{
	return ray->origin + ray->dir * dist;
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

bool intersect_plane(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// is there point copying object to local memory? we only access is 4-6 times
	// const t_object obj = objects[id];
	// converting ray to object's local coord system
	t_ray ray_local = ray2local(ray, &objects[id]);
	
	// check if array of points has space for new addition
	if ((MAX_POINTS - hit->num_elements) < 1)
		return false;
	// temporary structure to store point of intersection
	t_point pt;																	

	// check if ray doesnt go in parallel to plane
	if (ray_local.dir.y != 0.0f)												
		pt.dist = -ray_local.origin.y / ray_local.dir.y;
	else
		return false;
	// check if intersection didn't occure behind camera
	if (pt.dist <= 0)											
		return false;

	pt.obj_id = id;
	pt.pos = ray_local.origin + ray_local.dir * pt.dist;
	// "inside" determines 'type' of intersection. if we enter 'volume' of primitive inside is TRUE, if we exit - FALSE
	// since plane doesn't have volume we choose intersection from one side (top) as 'entrance' and opposite - 'exit'
	// determining 'type' of intersection is necessary for boolean operations on objects
	pt.normal = (float3)(0.0f, 1.0f, 0.0f);
	pt.inside = (dot(ray_local.dir, pt.normal) <= 0.f) ? true : false; 
	// this code is omitted for shadows, cause we dont need normals there, thus saving us some unnecessary calculations
	if (!quick)
	{
		pt.normal = dot(pt.normal, ray_local.dir) < 0.0f ? pt.normal : -pt.normal;
		pt.normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt.normal, 0.0f)).xyz;
		pt.normal = fast_normalize(pt.normal);
	}
	// coverting point to ray coord system
	pt.pos = mat_mult_vec(objects[id].from_local, (float4)(pt.pos, 1.0f)).xyz;
	// distance to point in ray coord system		
	pt.dist = length(pt.pos - ray->origin);
	// point is written into array of points intersected by current ray
	hit->pt[hit->num_elements++] = pt;
	return true;
}

bool intersect_sphere(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	// sphere has 2 intersection points (rarely one, when ray is tangent to sphere), thus we need to store 2 points
	t_point pt[2];

	float b = dot(-ray_local.origin, ray_local.dir);
	float c = dot(-ray_local.origin, -ray_local.origin) - 0.25f;
	float disc = b * b - c;
	// no solution for quadratic equation - no intersection
	if (disc < 0.0f)					
		return false;

	disc = sqrt(disc);
	pt[0].dist = b - disc;
	pt[1].dist = b + disc;
	// both intersections occured behind camera
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	// first intersection is valid
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].normal = pt[0].pos;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0.f) ? true : false; 
		if (!quick)
		{
			// inverting normal in case it 'looks' in 'same' direction as ray ('same' term is used very loosly here)
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	// second intersection is valid
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].normal = pt[1].pos;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0.f) ? true : false; 
		if (!quick)
		{	
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}

bool intersect_cylinder(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float a = 1.0f - ray_local.dir.y * ray_local.dir.y;
	float b = dot(ray_local.dir, -ray_local.origin) + ray_local.dir.y * ray_local.origin.y;
	float c = dot(ray_local.origin, ray_local.origin) - ray_local.origin.y * ray_local.origin.y - 0.25f;
	float disc = b * b - a * c;

	if (disc < 0.0f || a == 0.0f)															
	{
		if (objects[id].capped && ray_local.dir.y != 0.0f)
		{
			pt[0].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
			pt[1].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
			pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
			pt[1].normal = (float3)(0.0f, 1.0f, 0.0f);
			float3 tmp;
			tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
			if (dot(tmp, tmp) > 0.25f)
				pt[0].dist = 0.0f;
			tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
			if (dot(tmp, tmp) > 0.25f)
				pt[1].dist = 0.0f;
		}
		else
			return false;
	}
	else
	{
		disc = sqrt(disc);
		pt[0].dist = (b - disc) / a;
		pt[1].dist = (b + disc) / a;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[0].normal = (float3)(pt[0].pos.x, 0.0f, pt[0].pos.z);
		pt[1].normal = (float3)(pt[1].pos.x, 0.0f, pt[1].pos.z);

		if (objects[id].capped)
		{
			float m0, m1;
			m0 = ray_local.origin.y + ray_local.dir.y * pt[0].dist;
			m1 = ray_local.origin.y + ray_local.dir.y * pt[1].dist;
			if (ray_local.dir.y != 0.0f)
			{
				float3 tmp;
				if (m0 < -0.5f)
				{
					pt[0].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[0].dist = 0.0f;
					pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
				}
				else if (m0 > 0.5f)
				{
					pt[0].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[0].dist = 0.0f;
					pt[0].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				if (m1 < -0.5f)
				{
					pt[1].dist = (-0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, -0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[1].dist = 0.0f;
					pt[1].normal = (float3)(0.0f, -1.0f, 0.0f);
				}
				else if (m1 > 0.5f)
				{
					pt[1].dist = (0.5f - ray_local.origin.y) / ray_local.dir.y;
					tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.0f, 0.5f, 0.0f);
					if (dot(tmp, tmp) > 0.25f)
						pt[1].dist = 0.0f;
					pt[1].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
			}
			else
			{
				if (m0 < -0.5f || m0 > 0.5f)
					pt[0].dist = 0.0f;
				if (m1 < -0.5f || m1 > 0.5f)
					pt[1].dist = 0.0f;
			}
		}
	}
	
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) < 0) ? true : false; 
		if (!quick)
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) < 0) ? true : false; 
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}

bool intersect_cone(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit) // CLEAN UP CODE!
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	
	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float k_k = pow(tan(radians(26.565051f)), 2); 								
	float a = (1.0f - (1.0f + k_k) * ray_local.dir.y * ray_local.dir.y);
	float b = dot(ray_local.dir, -ray_local.origin) - (1 + k_k) * ray_local.dir.y * -ray_local.origin.y;
	float c = dot(-ray_local.origin, -ray_local.origin) - (1.0f + k_k) * ray_local.origin.y * ray_local.origin.y;
	float disc = b * b - a * c;
	
	if (disc < 0.0f || a == 0.0f)
		return false;
	disc = sqrt(disc);
	pt[0].dist = (b - disc) / a;
	pt[1].dist = (b + disc) / a;
	float m0 = ray_local.dir.y * pt[0].dist + ray_local.origin.y;
	float m1 = ray_local.dir.y * pt[1].dist + ray_local.origin.y;
	pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
	pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
	pt[0].normal = pt[0].pos - (float3)(0.f, m0 * (1 + k_k), 0.f);
	pt[1].normal = pt[1].pos - (float3)(0.f, m1 * (1 + k_k), 0.f);
	
	if (objects[id].capped)
	{
		if (ray_local.dir.y != 0.0f)
		{
			float3 tmp;
			if (m0 < -1.0f || m0 > 0.0f)
			{
				pt[0].dist = (-1.0f - ray_local.origin.y) / ray_local.dir.y;
				tmp = (ray_local.origin + pt[0].dist * ray_local.dir) - (float3)(0.f, -1.0f, 0.f);
				if (dot(tmp, tmp) > 0.25f)
					pt[0].dist = 0.0f;
				pt[0].normal = (float3)(0.0f, -1.0f, 0.0f);
			}
			if (m1 < -1.0f || m1 > 0.0f)
			{
				pt[1].dist = (-1.0f - ray_local.origin.y) / ray_local.dir.y;
				tmp = (ray_local.origin + pt[1].dist * ray_local.dir) - (float3)(0.f, -1.0f, 0.f);
				if (dot(tmp, tmp) > 0.25f)
					pt[1].dist = 0.0f;
				pt[1].normal = (float3)(0.0f, -1.0f, 0.0f);
			}
		}
		else
		{
			if (m0 < -1.0f || m0 > 0.0f)
				pt[0].dist = 0.0f;
			if (m1 < -1.0f || m1 > 0.0f)
				pt[1].dist = 0.0f;
		}
	}
	if (pt[0].dist <= 0.0f && pt[1].dist <= 0.0f)
		return false;
	if (pt[0].dist > 0)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}


bool intersect_box(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);

	if ((MAX_POINTS - hit->num_elements) < 2)
		return false;
	
	t_point pt[2];

	float3	minp = (float3)(-0.5f, -0.5f, 0.5f);								// closest bottom left point of box
	float3	maxp = (float3)(0.5f, 0.5f, -0.5f);									// furthest top right point of box 
	float3	ray_inv = (float3)(1.0f, 1.0f, 1.0f) / ray_local.dir;				// inverse of ray

	// intersecting "slabs". cube volume consists of volumes defined by 3 pairs of parallel planes
	// at each step we detemine whether our intersection point of plane lies within other 'slab' defined by pair of planes
	// more details in following article https://tavianator.com/fast-branchless-raybounding-box-intersections/

	float t1 = (minp.x - ray_local.origin.x) * ray_inv.x;
	float t2 = (maxp.x - ray_local.origin.x) * ray_inv.x;
	
	pt[0].dist = min(t1, t2);
	pt[1].dist = max(t1, t2);
	
	t1 = (minp.y - ray_local.origin.y) * ray_inv.y;
	t2 = (maxp.y - ray_local.origin.y) * ray_inv.y;
	
	pt[0].dist = max(pt[0].dist, min(min(t1, t2), pt[1].dist));
	pt[1].dist = min(pt[1].dist, max(max(t1, t2), pt[0].dist));
	
	t1 = (minp.z - ray_local.origin.z) * ray_inv.z;
	t2 = (maxp.z - ray_local.origin.z) * ray_inv.z;
	
	pt[0].dist = max(pt[0].dist, min(min(t1, t2), pt[1].dist));
	pt[1].dist = min(pt[1].dist, max(max(t1, t2), pt[0].dist));
	
	if (pt[1].dist <= max(pt[0].dist, 0.0f))
		return false;

	float3 d = (minp - maxp) * 0.5f;
	// depending on scale of object may cause "artifacts"
	float bias = 1.0001f;
	if (pt[0].dist > 0.0f)
	{
		pt[0].obj_id = objects[id].id;
		pt[0].pos = ray_local.origin + ray_local.dir * pt[0].dist;
		// trick to finding AABB (axis aligned bounding box) normal vector used from here
		// http://blog.johnnovak.net/2016/10/22/the-nim-raytracer-project-part-4-calculating-box-normals/
		pt[0].normal = pt[0].pos;
		pt[0].normal.x = (int)(pt[0].normal.x / fabs(d.x) * bias);
		pt[0].normal.y = (int)(pt[0].normal.y / fabs(d.y) * bias);
		pt[0].normal.z = (int)(pt[0].normal.z / fabs(d.z) * bias);
		pt[0].inside = (dot(ray_local.dir, pt[0].normal) <= 0) ? true : false; 
		if (!quick)
																				
		{
			pt[0].normal = dot(pt[0].normal, ray_local.dir) < 0.0f ? pt[0].normal : -pt[0].normal;
			pt[0].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[0].normal, 0.0f)).xyz;
			pt[0].normal = fast_normalize(pt[0].normal);
		}
		pt[0].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[0].pos, 1.0f)).xyz;
		pt[0].dist = length(pt[0].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[0];
	}
	if (pt[1].dist > 0.0f)
	{
		pt[1].obj_id = objects[id].id;
		pt[1].pos = ray_local.origin + ray_local.dir * pt[1].dist;
		pt[1].normal = pt[1].pos;
		pt[1].normal.x = (int)(pt[1].normal.x / fabs(d.x) * bias);
		pt[1].normal.y = (int)(pt[1].normal.y / fabs(d.y) * bias);
		pt[1].normal.z = (int)(pt[1].normal.z / fabs(d.z) * bias);
		pt[1].inside = (dot(ray_local.dir, pt[1].normal) <= 0) ? true : false;
		if (!quick)
		{
			pt[1].normal = dot(pt[1].normal, ray_local.dir) < 0.0f ? pt[1].normal : -pt[1].normal;
			pt[1].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(pt[1].normal, 0.0f)).xyz;
			pt[1].normal = fast_normalize(pt[1].normal);
		}
		pt[1].pos = mat_mult_vec(objects[id].from_local, (float4)(pt[1].pos, 1.0f)).xyz;
		pt[1].dist = length(pt[1].pos - ray->origin);
		hit->pt[hit->num_elements++] = pt[1];
	}
	return true;
}

bool intersect_union(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	switch(objects[objects[id].operand.x].type)
	{
		case PLANE :
			hit_x = intersect_plane(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_x = intersect_sphere(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_x = intersect_cylinder(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CONE :
			hit_x = intersect_cone(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case BOX :
			hit_x = intersect_box(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
	}
	switch(objects[objects[id].operand.y].type)
	{
		case PLANE :
			hit_y = intersect_plane(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_y = intersect_sphere(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_y = intersect_cylinder(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CONE :
			hit_y = intersect_cone(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case BOX :
			hit_y = intersect_box(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
	}
	if (!hit_x && !hit_y)
		return false;
	hitpoints_sort(hit);
	bool state_x, state_y = false;
	if (hit_x)
	{
		for (int i = 0; i < hit->num_elements; i++)
		// for (int i = 0; i < MAX_POINTS; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				// determining the initial state before first intersection with first object (whether ray was out of object or inside)
				state_x = !hit->pt[i].inside;
				break;
			}
		}	
	}
	if (hit_y)
	{	
		for (int i = 0; i < hit->num_elements; i++)
		// for (int i = 0; i < MAX_POINTS; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.y)
			{
				// determining the initial state before first intersection with second object (whether ray was out of object or inside)
				state_y = !hit->pt[i].inside;
				break;
			}
		}
	}
	// determining whether to keep or delete points from first object or second
	// for (int i = 0; i < MAX_POINTS; i++)
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			if (!state_y)
			{
				hit->pt[i].obj_id = objects[id].id;
				// hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
			
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			if (!state_x)
			{
				hit->pt[i].obj_id = objects[id].id;
				// hit->pt[i].inside = state_y ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}


bool intersect_intersection(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	switch(objects[objects[id].operand.x].type)
	{
		case PLANE :
			hit_x = intersect_plane(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_x = intersect_sphere(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_x = intersect_cylinder(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CONE :
			hit_x = intersect_cone(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case BOX :
			hit_x = intersect_box(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
	}
	switch(objects[objects[id].operand.y].type)
	{
		case PLANE :
			hit_y = intersect_plane(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_y = intersect_sphere(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_y = intersect_cylinder(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CONE :
			hit_y = intersect_cone(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case BOX :
			hit_y = intersect_box(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
	}
	if (!hit_x || !hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x || hit->pt[i].obj_id == objects[id].operand.y)
				hit->pt[i].obj_id = -1;
		}
		return false;
	}
	hitpoints_sort(hit);
	bool state_x, state_y = false;
	for (int i = 0; i < hit->num_elements; i++)
	{
		// determining the initial state before first intersection with first object (whether ray was out of object or inside)
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = !hit->pt[i].inside;
			break;
		}
	}	
	for (int i = 0; i < hit->num_elements; i++)
	{
		// determining the initial state before first intersection with second object (whether ray was out of object or inside)
		if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = !hit->pt[i].inside;
			break;
		}
	}
	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_y)
			{
				hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
			
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_x)
			{
				hit->pt[i].inside = state_y ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}

bool intersect_difference(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	switch(objects[objects[id].operand.x].type)
	{
		case PLANE :
			hit_x = intersect_plane(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_x = intersect_sphere(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_x = intersect_cylinder(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CONE :
			hit_x = intersect_cone(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case BOX :
			hit_x = intersect_box(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
	}
	// if there is no intersection with first object of DIFFERENCE - ignore all the rest
	if (!hit_x)
		return false;
	switch(objects[objects[id].operand.y].type)
	{
		case PLANE :
			hit_y = intersect_plane(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_y = intersect_sphere(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_y = intersect_cylinder(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case CONE :
			hit_y = intersect_cone(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		case BOX :
			hit_y = intersect_box(objects, objects[id].operand.y, &ray_local, quick, hit);
			break;
		// case UNION :
		// 	hit_y = intersect_union(objects, obj.operand.y, &ray_local, quick, hit);
		// 	break;
		// case DIFFERENCE :
		// 	hit_y = intersect_difference(objects, obj.operand.y, &ray_local, quick, hit);
		// 	break;
		// case INTERSECTION :
		// 	hit_y = intersect_intersection(objects, obj.operand.y, &ray_local, quick, hit);
		// 	break;
	}
	//if there is intersection with first object but not second - clean up points generated by first object
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;
	}
	hitpoints_sort(hit);
	bool state_x, state_y;
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = !hit->pt[i].inside;
			break;
		}
	}
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = !hit->pt[i].inside;
			break;
		}
	}
	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (!state_y)
			{	
				hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;

		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (state_x)
			{
				hit->pt[i].inside = state_y ? false : true;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
	}
	return true;
}

bool intersect_clipping(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{	
	// const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &objects[id]);
	bool hit_x, hit_y = false;

	switch(objects[objects[id].operand.x].type)
	{
		case PLANE :
			hit_x = intersect_plane(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case SPHERE :
			hit_x = intersect_sphere(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CYLINDER :
			hit_x = intersect_cylinder(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case CONE :
			hit_x = intersect_cone(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
		case BOX :
			hit_x = intersect_box(objects, objects[id].operand.x, &ray_local, quick, hit);
			break;
	}
	// if there is no intersection with first object of DIFFERENCE - ignore all the rest
	if (!hit_x)
		return false;

	if (objects[objects[id].operand.y].type == PLANE)
	{
		float3 plane_normal = mat_mult_vec(mat_transpose(objects[objects[id].operand.y].to_local), (float4)(0.0f, 1.0f, 0.0f, 0.0f)).xyz;
		float3 plane_pos = mat_mult_vec(objects[objects[id].operand.y].from_local, (float4)(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
		plane_normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(plane_normal, 0.0f)).xyz;
		plane_pos = mat_mult_vec(objects[id].from_local, (float4)(plane_pos, 1.0f)).xyz;
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
				if (dot(plane_pos - hit->pt[i].pos, plane_normal) < 0.0f)
					hit->pt[i].obj_id = -1;
			}
		}
		return true;
	}
	else
		switch(objects[objects[id].operand.y].type)
		{
			case SPHERE :
				hit_y = intersect_sphere(objects, objects[id].operand.y, &ray_local, quick, hit);
				break;
			case CYLINDER :
				hit_y = intersect_cylinder(objects, objects[id].operand.y, &ray_local, quick, hit);
				break;
			case CONE :
				hit_y = intersect_cone(objects, objects[id].operand.y, &ray_local, quick, hit);
				break;
			case BOX :
				hit_y = intersect_box(objects, objects[id].operand.y, &ray_local, quick, hit);
				break;
			default :
				hit_y = false;
		}
	//if there is intersection with first object but not second - clean up points generated by first object
	bool state_y = false;
	if (!hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.x)
			{
				hit->pt[i].obj_id = objects[id].id;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
		}
		return true;	
	}
	hitpoints_sort(hit);
	if (hit_y)
	{
		for (int i = 0; i < hit->num_elements; i++)
		{
			if (hit->pt[i].obj_id == objects[id].operand.y)
			{
				state_y = !hit->pt[i].inside;
				break;
			}
		}
	}

	// determining whether to keep or delete points from first object or second
	for (int i = 0; i < hit->num_elements; i++)
	{
		if (hit->pt[i].obj_id == objects[id].operand.x)
		{
			// state_x = hit->pt[i].inside;
			hit->pt[i].obj_id = objects[id].id;
			if (!state_y)
			{	
				// hit->pt[i].inside = state_x ? true : false;
				if (!quick)
				{
					// hit->pt[i].normal = dot(hit->pt[i].normal, ray_local.dir) < 0.0f ? hit->pt[i].normal : -hit->pt[i].normal;
					hit->pt[i].normal = mat_mult_vec(mat_transpose(objects[id].to_local), (float4)(hit->pt[i].normal, 0.0f)).xyz;
					hit->pt[i].normal = fast_normalize(hit->pt[i].normal);
				}
				hit->pt[i].pos = mat_mult_vec(objects[id].from_local, (float4)(hit->pt[i].pos, 1.0f)).xyz;
				hit->pt[i].dist = length(hit->pt[i].pos - ray->origin);
			}
			else
				hit->pt[i].obj_id = -1;
		}
		else if (hit->pt[i].obj_id == objects[id].operand.y)
		{
			state_y = hit->pt[i].inside;
			hit->pt[i].obj_id = -1;
		}
	}
	return true;
}
/*
bool intersect_bocal(__constant t_object *objects, int id, const t_ray *ray, bool quick, t_hitpoints *hit)
{
	const t_object obj = objects[id];
	t_ray ray_local = ray2local(ray, &obj);

	if ((MAX_POINTS - hit->num_elements) < 6)
		return false;
	t_hitpoints bocal_hit;
	hitpoints_init(&bocal_hit);
	int hit_number = 0;

	if (intersect_difference(compound, 2, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_box(compound, 3, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_difference(compound, 4, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_union(compound, 7, &ray_local, quick, &bocal_hit))
		hit_number++;
	if (intersect_union(compound, 8, &ray_local, quick, &bocal_hit))
		hit_number++;

	if (intersect_difference(compound, 2, &ray_local, quick, &bocal_hit))
	{
		int i = 0;
		while (i != bocal_hit.num_elements - 1)
		{
			bocal_hit.pt[i].obj_id = id;
			hit->pt[hit->num_elements++] = bocal_hit.pt[i];
			++i;
		}
		return true;
	}
	return false;
}
*/
bool intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray,	float3 *normal, float *t, int *object_id, bool quick)
{
	// initialise t to a very large number, so t will be guaranteed to be smaller	when a hit with the scene occurs
	float inf = 1e20f;
	// float hitdistance = 0.0f;
	int	hit_occured = 0;
	// float3 loc_normal = (float3)(0.0f, 0.0f, 0.0f);
	*t = inf;
	t_hitpoints hit;
	hitpoints_init(&hit);

	// check each object if the ray intersects it in the scene
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

	if (hit.pt[0].obj_id == -1 || hit.pt[0].dist <= 0.0f)
		return false;

	*t = hit.pt[0].dist;
	*normal = hit.pt[0].normal;
	*object_id = hit.pt[0].obj_id;

	return true;
}

/* the path tracing function */
/* computes a path (starting from the camera) with a defined number of bounces, accumulates light/color at each bounce */
/* each ray hitting a surface will be reflected in a random direction (by randomly sampling the hemisphere above the hitpoint) */
/* small optimisation: diffuse ray directions are calculated using cosine weighted importance sampling */



// float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int object_count, const int *seed0, const int *seed1)
// {
// 	t_ray ray = *camray;

// 	float3 accum_color = (float3)(0.0f, 0.0f, 0.0f);
// 	float3 mask = (float3)(1.0f, 1.0f, 1.0f);
// 	float3 normal_facing = (float3)(0.0f, 0.0f, 0.0f);

// 	for (int bounces = 0; bounces < NUM_BOUNCES; bounces++)
// 	{
// 		// distance to intersection
// 		float t;
// 		// index of intersected sphere
// 		int hitobject_id = 0; 

// 		// if ray misses scene, return background colour
// 		if (!intersect_scene(spheres, object_count, &ray, &normal_facing, &t, &hitobject_id, false))
// 			return accum_color += mask * (float3)(0.15f, 0.15f, 0.25f);

// 		// else, we've got a hit! Fetch the closest hit sphere
// 		//version with local copy of sphere
// 		t_object hitobject = spheres[hitobject_id]; 

// 		//compute the hitpoint using the ray equation
// 		float3 hitpoint = ray.origin + ray.dir * t;
		
// 		//compute the surface normal and flip it if necessary to face the incoming ray
// 		// float3 normal_facing = find_normal(&hitobject, &ray, t);

// 		//compute two random numbers to pick a random point on the hemisphere above the hitpoint
// 		float rand1 = 2.0f * PI * get_random(seed0, seed1);
// 		float rand2 = get_random(seed0, seed1);
// 		float rand2s = sqrt(rand2);

// 		//create a local orthogonal coordinate frame centered at the hitpoint
// 		float3 w = normal_facing;
// 		float3 axis = fabs(w.x) > 0.1f ? (float3)(0.0f, 1.0f, 0.0f) : (float3)(1.0f, 0.0f, 0.0f);
// 		float3 u = normalize(cross(axis, w));
// 		float3 v = cross(w, u);

// 		//use the coordinte frame and random numbers to compute the next ray direction
// 		float3 newdir = normalize(u * cos(rand1)*rand2s + v*sin(rand1)*rand2s + w*sqrt(1.0f - rand2));

// 		//add a very small offset to the hitpoint to prevent self intersection
// 		ray.origin = hitpoint + normal_facing * EPSILON;
// 		ray.dir = newdir;

// 		//add the colour and light contributions to the accumulated colour
// 		accum_color += mask * hitobject.emi; 

// 		//the mask colour picks up surface colours at each bounce
// 		mask *= hitobject.color; 
		
// 		// perform cosine-weighted importance sampling for diffuse surfaces
// 		mask *= dot(newdir, normal_facing);
// 	}
// 	return accum_color;
// }




// float3 get_diffuse_light(__constant t_object *objects, const int num_objects, float3 hitpoint, int h_id, float3 normal,
// 						 __constant t_light *lights, const int num_lights)
// {
// 	float3	diffuse = (float3)(0.0f, 0.0f, 0.0f);
// 	float3	dummy_normal;
// 	float	dist_to_light = 0;
// 	float	t = 1e20f;
// 	int		dummy_id = 0;
// 	t_ray	shadow_ray;

// 	shadow_ray.origin = hitpoint + normal * EPSILON;

// 	for (int i = 0; i < num_lights; i++)
// 	{
// 		shadow_ray.dir = fast_normalize(lights[i].location - shadow_ray.origin);
// 		dist_to_light = fast_length(lights[i].location - shadow_ray.origin);

// 		if (intersect_scene(objects, num_objects, &shadow_ray, &dummy_normal, &t, &dummy_id, true) &&
// 			t < dist_to_light)
// 			continue;

// 		float intensity = zero_clamp(dot(normal, shadow_ray.dir));

// 		diffuse += intensity * lights[i].emission * objects[h_id].color;
// 	}
// 	return diffuse;
// }

float3 get_direct_light(__constant t_object *objects, const t_ray *camray, const int object_count,
						__constant t_light *lights, const int num_lights)
{
	t_ray ray = *camray;

	float3 bg_color = (float3)(1.0f, 1.0f, 1.0f);
	float3 hit_color = (float3)(0.0f, 0.0f, 0.0f);
	float3 diffuse = (float3)(0.0f, 0.0f, 0.0f);
	float3 specular = (float3)(0.0f, 0.0f, 0.0f);
	float3 normal = (float3)(0.0f, 0.0f, 0.0f);
	float3 dummy_normal;

	float t = 1e20f;   /* distance to intersection */
	int hitobject_id = 0; /* index of intersected sphere */
	int dummy_id = 0;

	/* if ray misses scene, return background colour */
	if (!intersect_scene(objects, object_count, &ray, &normal, &t, &hitobject_id, false))
		return bg_color;

	/* else, we've got a hit! Fetch the closest hit sphere */
	t_object hitobject = objects[hitobject_id]; /* version with local copy of sphere */

	diffuse = hitobject.color * 0.2f;
	/* compute the hitpoint using the ray equation */
	float3 hitpoint = hitpoint_calc(&ray, t);

	// -------------------------------------------------
	/* add a very small offset to the hitpoint to prevent self intersection */
	ray.origin = hitpoint + normal * EPSILON;

	float dist_to_light = 0;

	for (int i = 0; i < num_lights; i++)
	{
		ray.dir = fast_normalize(lights[i].location - ray.origin);
		dist_to_light = fast_length(lights[i].location - ray.origin);

		if (intersect_scene(objects, object_count, &ray, &dummy_normal, &t, &dummy_id, true) && t < dist_to_light)
			continue;

		//spot light shading
		float intensity = zero_clamp(dot(normal, ray.dir));
		diffuse += intensity * 0.8f * lights[i].emission * hitobject.color;

		float3 refl_dir = fast_normalize((2 * intensity * normal) - ray.dir);

		float phong_term = zero_clamp(dot(refl_dir, (-1.0f) * camray->dir));
		if (intensity == 0.0f)
			phong_term = 0.0f;

		specular += lights[i].emission * pow(phong_term, hitobject.specular_exp);
	}

 	hit_color = hitobject.diffuse * diffuse + hitobject.specular * specular;

 	// reflection
	// t_ray refl_ray;
	// refl_ray.origin = ray.origin;

	// float c1 = (-1.0f) * dot(normal, camray->dir);
	// refl_ray.dir = camray->dir + 2 * normal * c1;

	// if (intersect_scene(objects, object_count, &refl_ray, &dummy_normal, &t, &hitobject_id, true))
	// 	hit_color += 0.5f * objects[hitobject_id].color;

	return hit_color;
}

__kernel void render_scene(__constant t_object *objects, const int object_count,
					__constant t_light *lights, const int num_lights,
					__global uchar4 *output, const int width, const int height,
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

	float3 pixel_float = get_direct_light(objects, &camray, object_count, lights, num_lights);

	// ----------- PATH TRACING! -----------
	// for (int i = 0; i < SAMPLES; i++)
	// 	finalcolor += trace_path(spheres, &camray, object_count, &seed0, &seed1) * invSamples;

	output[work_item_id].z = (uchar)(clamp(pixel_float.x, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].y = (uchar)(clamp(pixel_float.y, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].x = (uchar)(clamp(pixel_float.z, 0.0f, 1.0f) * 255 + .5f);
	output[work_item_id].w = 1;
}
