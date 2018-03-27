/* required to compensate for limited float precision */
// __constant float EPSILON = 0.00003f;
__constant float EPSILON = 0.005f;
__constant float PI = 3.14159265359f;
__constant int SAMPLES = 500;
__constant int NUM_BOUNCES = 7;

# define PLANE		0
# define SPHERE		1
# define CYLINDER	2
# define CONE		3
# define BOX		4
# define MERGE		5
# define XSECT		6
# define MINUS		7
# define CLIP		8

typedef struct	s_ray
{
	float3 origin;
	float3 dir;
}				t_ray;

typedef float16 mat4;

typedef struct	s_hitpoint
{
	int		obj_id;
	float3	pos;
	float3	normal;
	float	dist;
	bool	inside;

}				t_hitpoint;

typedef struct	s_object
{
	int		id;
	// int		inherits_from;
	int		type;
	// int2	operand;
	bool	capped;
	float3	loc;
	float3	rot;
	float3	sca;
	float3	color;
	float3	emi;
	float	diffuse;
	float	specular;
	float	specular_exp;
	mat4 from_local;
	mat4 to_local;
	// char	rot_change;
	// char	tra_change;
	// char	sca_change;
}				t_object;

typedef struct	s_light
{
	float3 loc;
	float3 emission;
}				t_light;

typedef struct	s_camera
{
	float3		loc;
	float3		rot;
	float		fov;
	mat4		trans_matrix;
	mat4		rot_matrix;
}				t_camera;


static float get_random(unsigned int *seed0, unsigned int *seed1);
static float zero_clamp(float x);
t_ray create_cam_ray(__constant t_camera *camera, const int x_coord, const int y_coord, const int width, const int height);
float3 trace_path(__constant t_object *spheres, const t_ray *camray, const int sphere_count, const int *seed0, const int *seed1);
float3 get_direct_light(__constant t_object *spheres, const t_ray *camray, const int sphere_count, __constant t_light *lights, const int num_lights);

// -------------------------- Object intersections ----------------------------
float4	intersect_sphere(const t_object *obj, const t_ray *ray, bool quick);
float4	intersect_plane(const t_object *obj, const t_ray *ray, bool quick);
float4	intersect_cylinder(const t_object *obj, const t_ray *ray, bool quick);
float4	intersect_cone(const t_object *obj, const t_ray *ray, bool quick);
float4	intersect_box(const t_object *obj, const t_ray *ray, bool quick);
bool	intersect_scene(__constant t_object *objects, const int num_objects,
					const t_ray *ray, float3 *normal, float *t, int *object_id, bool quick);

// ---------------------------- Object normals --------------------------------
// float3	normal_sphere(const t_object *obj, const t_ray *ray, float dist);
// float3	normal_plane(const t_object *obj, const t_ray *ray);
// float3	normal_cylinder(const t_object *obj, const t_ray *ray, float dist);
// float3	normal_cone(const t_object *obj, const t_ray *ray, float dist);
// float3	normal_box(const t_object *obj, const t_ray *ray, float dist);

float3	hitpoint_calc(const t_ray *ray, float dist);
// void	update_transform(t_object *obj);
mat4	mat_transform(const t_object *obj);
mat4	mat_translate(float3 v);
mat4	mat_translate_inv(float3 v);
mat4	mat_scale(float3 s);
mat4	mat_scale_inv(float3 s);
mat4	mat_rotx(float angle);
mat4	mat_roty(float angle);
mat4	mat_rotz(float angle);
mat4	mat_rota(float angle, float3 axis);
mat4	mat_rotx_inv(float angle);
mat4	mat_roty_inv(float angle);
mat4	mat_rotz_inv(float angle);
mat4	mat_rota_inv(float angle, float3 axis);
mat4	mat_invert(mat4 m);
float4	mat_mult_vec(mat4 m, float4 v);
mat4	mat_transpose(mat4 m);
mat4	mat_mult_mat(mat4 a, mat4 b);
void	hitpoints_init(t_hitpoint* hit, int id, int size);
void	hitpoints_sort(t_hitpoint* hit, int size);
void 	hitpoints_from_local(t_hitpoint* hit, int size, const t_object *obj, const t_ray *ray, bool quick);
t_ray	ray2local(const t_ray *r, const t_object *o);


// void	update_transform(t_object *obj)
// {
// 	mat4 tra, rot, sca = (float16)(0.f);
// 	int	flag = 0;
// 	if (obj->tra_change != 0)
// 	{
// 		tra = mat_translate(obj->pos);
// 		flag += 1;
// 		obj->tra_change = 0;
// 	}
// 	if (obj->rot_change != 0)
// 	{
// 		rot = mat_rotx(obj->rot.x);
// 		rot = mat_mult_mat(mat_roty(obj->rot.y), rot);
// 		rot = mat_mult_mat(mat_rotz(obj->rot.z), rot);
// 		flag += 2;
// 		obj->rot_change = 0;
// 	}
// 	if (obj->sca_change != 0)
// 	{
// 		sca = mat_scale(obj->sca);
// 		flag += 4;
// 		obj->sca_change = 0;
// 	}
// 	switch (flag)
// 	{
// 		case 0:
// 			break;
// 		case 1:
// 			obj->from_local = tra;
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 2:
// 			obj->from_local = rot;
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 3:
// 			obj->from_local = mat_mult_mat(tra,rot);
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 4:
// 			obj->from_local = sca;
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 5:
// 			obj->from_local = mat_mult_mat(tra, sca);
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 6:
// 			obj->from_local = mat_mult_mat(rot, sca);
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 		case 7:
// 			obj->from_local = mat_mult_mat(mat_mult_mat(tra, rot), sca);
// 			obj->to_local = mat_invert(from_local);
// 			break;
// 	}
// }

t_ray ray2local(const t_ray *r, const t_object *o)
{
	t_ray ray;

	ray.dir = fast_normalize(mat_mult_vec(o->to_local, (float4)(r->dir, 0.0f)).xyz);
	ray.origin = mat_mult_vec(o->to_local, (float4)(r->origin, 1.0f)).xyz;
	return ray;
}

void hitpoints_init(t_hitpoint* hit, int id, int size)
{
	for (int i = 0; i < size; i++)
	{
		hit[i].obj_id = id;
		hit[i].dist = 0.0f;
		hit[i].pos = (float3)(0.0f);
		hit[i].normal = (float3)(0.0f);
		hit[i].inside = false;
	}
}

void hitpoints_sort(t_hitpoint* hit, int size)
{
	int i = 0;
	t_hitpoint tmp;
	while (i != size - 1)
	{
		if (hit[i + 1].dist > EPSILON && (hit[i + 1].dist < hit[i].dist || hit[i].dist <= EPSILON))
		{
			tmp = hit[i];
			hit[i] = hit[i + 1];
			hit[i + 1] = tmp;
			i = 0;
		}
		else
			i++;
	}
}

void hitpoints_from_local(t_hitpoint* hit, int size, const t_object *obj, const t_ray *ray, bool quick)
{
	for (int i = 0; i < size; i++)
	{
		if (hit[i].dist > 0.0f)
		{
			hit[i].pos = mat_mult_vec(obj->from_local, (float4)(hit[i].pos, 1.0f)).xyz;
			hit[i].dist = length(hit[0].pos - ray->origin);
			if (!quick)
			{
				hit[i].normal = mat_mult_vec(mat_transpose(obj->to_local), (float4)(hit[i].normal, 0.0f)).xyz;
				hit[i].normal = fast_normalize(hit[i].normal);
			}
		}
	}
}

mat4	mat_transform(const t_object *obj)
{
	mat4 tra, rot, sca = (float16)(0.f);
	tra = mat_translate(obj->loc);
	rot = mat_rotx(obj->rot.x);
	rot = mat_mult_mat(mat_roty(obj->rot.y), rot);
	rot = mat_mult_mat(mat_rotz(obj->rot.z), rot);
	sca = mat_scale(obj->sca);
	return (mat_mult_mat(mat_mult_mat(tra, rot), sca));
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

mat4 mat_rotx(float angle)
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

mat4 mat_roty(float angle)
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

mat4 mat_rotz(float angle)
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

mat4 mat_rota(float angle, float3 axis)
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

	// mat4 tra = mat_translate(camera->loc);
	// mat4 rot = mat_rotx(camera->rot.x);

	// rot = mat_mult_mat(mat_roty(camera->rot.y), rot);
	// rot = mat_mult_mat(mat_rotz(camera->rot.z), rot);
	mat4 txx = mat_mult_mat(camera->trans_matrix, camera->rot_matrix);
	
	ray.origin = mat_mult_vec(txx, (float4)(ray.origin, 1.0f)).xyz;
	ray.dir = fast_normalize(mat_mult_vec(txx, (float4)(ray.dir, 0.0f)).xyz);

	return ray;
}

/* version using local copy of sphere */
float4 intersect_sphere(const t_object *obj, const t_ray *ray, bool quick)
{
	t_ray ray_loc = ray2local(ray, obj);

	t_hitpoint hit[2];
	hitpoints_init(hit, obj->id, 2);

	//actual intersection calc
	float b = dot(-ray_loc.origin, ray_loc.dir);
	float c = dot(-ray_loc.origin, -ray_loc.origin) - 0.25f;
	float disc = b * b - c;

	if (disc < 0.0f)
		return (float4)(0.0f);
	disc = sqrt(disc);
	hit[0].dist = b - disc;
	hit[1].dist = b + disc;
	
	hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
	hit[1].pos = ray_loc.origin + ray_loc.dir * hit[1].dist;
	if (!quick)
	{	
		hit[0].normal = hit[0].pos;
		hit[0].normal = dot(hit[0].normal, ray_loc.dir) < 0.0f ? hit[0].normal : -hit[0].normal;
		hit[1].normal = hit[1].pos;
		hit[1].normal = dot(hit[1].normal, ray_loc.dir) < 0.0f ? hit[1].normal : -hit[1].normal;
	}
	hitpoints_from_local(hit, 2, obj, ray, quick);
	hitpoints_sort(hit, 2);
	if (hit[0].dist <= EPSILON)
		return (float4)(0.0f);
	return (float4)(hit[0].normal, hit[0].dist);
}

// bool intersect_plane(const t_object *obj, const t_ray *ray, bool quick, t_hitpoint *res)
float4 intersect_plane(const t_object *obj, const t_ray *ray, bool quick)
{
	t_ray ray_loc = ray2local(ray, obj);
	t_hitpoint hit[2];
	hitpoints_init(hit, obj->id, 2);

	//actual intersection calc
	hit[0].dist = 0.0f;
	if (ray_loc.dir.y != 0.0f)
		hit[0].dist = -ray_loc.origin.y / ray_loc.dir.y;
	else
		return (float4)(0.0f);
	if (!quick)
	{
		hit[0].normal = (float3)(0.0f, 1.0f, 0.0f);
		hit[0].normal = dot(hit[0].normal, ray_loc.dir) < 0.0f ? hit[0].normal : -hit[0].normal;
	}
	hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
	hitpoints_from_local(hit, 2, obj, ray, quick);
	if (hit[0].dist <= EPSILON)
		return (float4)(0.0f);
	return (float4)(hit[0].normal, hit[0].dist);
}



float4 intersect_cylinder(const t_object *obj, const t_ray *ray, bool quick)
{
	t_ray ray_loc = ray2local(ray, obj);
	t_hitpoint hit[2];
	hitpoints_init(hit, obj->id, 2);

	float a = 1.0f - ray_loc.dir.y * ray_loc.dir.y;
	float b = dot(ray_loc.dir, -ray_loc.origin) + ray_loc.dir.y * ray_loc.origin.y;
	float c = dot(ray_loc.origin, ray_loc.origin) - ray_loc.origin.y * ray_loc.origin.y - 0.25f;
	float disc = b * b - a * c;

	if (disc < 0.0f)
	{
		if (obj->capped && ray_loc.dir.y == 1.0f)
		{
			hit[0].dist = (-0.5f - ray_loc.origin.y) / ray_loc.dir.y;
			hit[1].dist = (0.5f - ray_loc.origin.y) / ray_loc.dir.y;
			hit[0].normal = (float3)(0.f, 1.f, 0.f);
			hit[1].normal = (float3)(0.f, 1.f, 0.f);
			float3 tmp;
			tmp = (ray_loc.origin + hit[0].dist * ray_loc.dir) - (float3)(0.f, -0.5f, 0.f);
			if (dot(tmp, tmp) > 0.25f)
				hit[0].dist = 0.0f;
			tmp = (ray_loc.origin + hit[1].dist * ray_loc.dir) - (float3)(0.f, -0.5f, 0.f);
			if (dot(tmp, tmp) > 0.25f)
				hit[1].dist = 0.0f;
		}
		else
			return (float4)(0.f);
	}
	else
	{
		disc = sqrt(disc);
		hit[0].dist = (b - disc) / a;
		hit[1].dist = (b + disc) / a;
		hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
		hit[1].pos = ray_loc.origin + ray_loc.dir * hit[1].dist;
		hit[0].normal = (float3)(hit[0].pos.x, 0.0f, hit[0].pos.z);
		hit[1].normal = (float3)(hit[1].pos.x, 0.0f, hit[1].pos.z);

		if (obj->capped)
		{
			float m1, m2;
			m1 = ray_loc.dir.y * hit[0].dist + ray_loc.origin.y;
			m2 = ray_loc.dir.y * hit[1].dist + ray_loc.origin.y;
			if (ray_loc.dir.y != 0.0f)
			{
				float3 tmp;
				if (m1 < -0.5f)
				{
					hit[0].dist = (-0.5f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[0].dist * ray_loc.dir) - (float3)(0.f, -0.5f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[0].dist = 0.0f;
					hit[0].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				else if (m1 > 0.5f)
				{
					hit[0].dist = (0.5f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[0].dist * ray_loc.dir) - (float3)(0.f, 0.5f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[0].dist = 0.0f;
					hit[0].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				if (m2 < -0.5f)
				{
					hit[1].dist = (-0.5f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[1].dist * ray_loc.dir) - (float3)(0.f, -0.5f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[1].dist = 0.0f;
					hit[1].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				else if (m2 > 0.5f)
				{
					hit[1].dist = (0.5f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[1].dist * ray_loc.dir) - (float3)(0.f, 0.5f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[1].dist = 0.0f;
					hit[1].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
			}
			else
			{
				if (m1 < -0.5f || m1 > 0.5f)
					hit[0].dist = 0.0f;
				if (m2 < -0.5f || m2 > 0.5f)
					hit[1].dist = 0.0f;
			}
		}
	}
	
	if (!quick)
	{
		hit[0].normal = dot(hit[0].normal, ray_loc.dir) < 0.0f ? hit[0].normal : -hit[0].normal;
		hit[1].normal = dot(hit[1].normal, ray_loc.dir) < 0.0f ? hit[1].normal : -hit[1].normal;
	}
	hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
	hit[1].pos = ray_loc.origin + ray_loc.dir * hit[1].dist;
	hitpoints_from_local(hit, 2, obj, ray, quick);
	hitpoints_sort(hit, 2);
	if (hit[0].dist <= EPSILON)
		return (float4)(0.0f);
	return (float4)(hit[0].normal, hit[0].dist);
}


float4 intersect_cone(const t_object *obj, const t_ray *ray, bool quick) // CLEAN UP CODE!
{
	t_ray ray_loc = ray2local(ray, obj);
	t_hitpoint hit[2];
	hitpoints_init(hit, obj->id, 2);

	float k_k = pow(tan(radians(26.5652f)), 2); // hardcode the number?
	float a = (1.0f - (1.0f + k_k) * ray_loc.dir.y * ray_loc.dir.y);
	float b = dot(ray_loc.dir, -ray_loc.origin) - (1 + k_k) * ray_loc.dir.y * -ray_loc.origin.y;
	float c = dot(-ray_loc.origin, -ray_loc.origin) - (1.0f + k_k) * ray_loc.origin.y * ray_loc.origin.y;
	float disc = b * b - a * c;
	
	if (disc < 0.0f)
		return (float4)(0.0f);
	else
	{
		disc = sqrt(disc);
		hit[0].dist = (b - disc) / a;
		hit[1].dist = (b + disc) / a;
		float m1 = ray_loc.dir.y * hit[0].dist + ray_loc.origin.y;
		float m2 = ray_loc.dir.y * hit[1].dist + ray_loc.origin.y;
		if (!quick)
		{
			hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
			hit[1].pos = ray_loc.origin + ray_loc.dir * hit[1].dist;
			hit[0].normal = hit[0].pos - (float3)(0.f, m1 * (1 + k_k), 0.f);
			hit[1].normal = hit[1].pos - (float3)(0.f, m2 * (1 + k_k), 0.f);
		}
		if (obj->capped)
		{
			if (ray_loc.dir.y != 0.0f)
			{
				float3 tmp;
				if (m1 > 0.0f)
					hit[0].dist = 0.0f;
				else if (m1 < -1.0f)
				{
					hit[0].dist = (-1.0f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[0].dist * ray_loc.dir) - (float3)(0.f, -1.0f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[0].dist = 0.0f;
					hit[0].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
				if (m2 > 0.0f)
					hit[1].dist = 0.0f;
				else if (m2 < -1.0f)
				{
					hit[1].dist = (-1.0f - ray_loc.origin.y) / ray_loc.dir.y;
					tmp = (ray_loc.origin + hit[1].dist * ray_loc.dir) - (float3)(0.f, -1.0f, 0.f);
					if (dot(tmp, tmp) > 0.25f)
						hit[1].dist = 0.0f;
					hit[1].normal = (float3)(0.0f, 1.0f, 0.0f);
				}
			}
			else
			{
				if (m1 < -1.0f || m1 > 0.0f)
					hit[0].dist = 0.0f;
				if (m2 < -1.0f || m2 > 0.0f)
					hit[1].dist = 0.0f;
			}
		}
	}
	hit[0].pos = ray_loc.origin + ray_loc.dir * hit[0].dist;
	hit[1].pos = ray_loc.origin + ray_loc.dir * hit[1].dist;
	if (!quick)
	{	
		hit[0].normal = dot(hit[0].normal, ray_loc.dir) < 0.0f ? hit[0].normal : -hit[0].normal;
		hit[1].normal = dot(hit[1].normal, ray_loc.dir) < 0.0f ? hit[1].normal : -hit[1].normal;
	}
	hitpoints_from_local(hit, 2, obj, ray, quick);
	hitpoints_sort(hit, 2);
	if (hit[0].dist <= EPSILON)
		return (float4)(0.0f);
	return (float4)(hit[0].normal, hit[0].dist);
}


float4 intersect_box(const t_object *obj, const t_ray *ray, bool quick)
{
	t_ray ray_loc = ray2local(ray, obj);

	t_hitpoint hit[2];
	hitpoints_init(hit, obj->id, 2);

	float3	minp = (float3)(-0.5f, -0.5f, 0.5f);
	float3	maxp = (float3)(0.5f, 0.5f, -0.5f);
	float3	ray_inv = (float3)(1.0f, 1.0f, 1.0f) / ray_loc.dir;

	// intersecting "slabs"

	float t1 = (minp.x - ray_loc.origin.x) * ray_inv.x;
	float t2 = (maxp.x - ray_loc.origin.x) * ray_inv.x;
	
	hit[0].dist = min(t1, t2);
	hit[1].dist = max(t1, t2);
	
	t1 = (minp.y - ray_loc.origin.y) * ray_inv.y;
	t2 = (maxp.y - ray_loc.origin.y) * ray_inv.y;
	
	hit[0].dist = max(hit[0].dist, min(min(t1, t2), hit[1].dist));
	hit[1].dist = min(hit[1].dist, max(max(t1, t2), hit[0].dist));
	
	t1 = (minp.z - ray_loc.origin.z) * ray_inv.z;
	t2 = (maxp.z - ray_loc.origin.z) * ray_inv.z;
	
	hit[0].dist = max(hit[0].dist, min(min(t1, t2), hit[1].dist));
	hit[1].dist = min(hit[1].dist, max(max(t1, t2), hit[0].dist));
	
	if (hit[1].dist <= max(hit[0].dist, 0.0f))
		return (float4)(0.0f);

	hit[0].pos = (ray_loc.origin + ray_loc.dir * hit[0].dist);
	hit[1].pos = (ray_loc.origin + ray_loc.dir * hit[1].dist);
	if (!quick)
	{
		hit[0].normal = hit[0].pos;
		hit[1].normal = hit[1].pos;
		float3 d = (minp - maxp) * 0.5f;
		float bias = 1.0001f; // depending on scale of object may cause "artifacts"
		hit[0].normal.x = (int)(hit[0].normal.x / fabs(d.x) * bias);
		hit[0].normal.y = (int)(hit[0].normal.y / fabs(d.y) * bias);
		hit[0].normal.z = (int)(hit[0].normal.z / fabs(d.z) * bias);
		hit[1].normal.x = (int)(hit[1].normal.x / fabs(d.x) * bias);
		hit[1].normal.y = (int)(hit[1].normal.y / fabs(d.y) * bias);
		hit[1].normal.z = (int)(hit[1].normal.z / fabs(d.z) * bias);
		hit[0].normal = dot(hit[0].normal, ray_loc.dir) < 0.0f ? hit[0].normal : -hit[0].normal;
		hit[1].normal = dot(hit[1].normal, ray_loc.dir) < 0.0f ? hit[1].normal : -hit[1].normal;
	}
	hitpoints_from_local(hit, 2, obj, ray, quick);
	hitpoints_sort(hit, 2);
	if (hit[0].dist <= EPSILON)
		return (float4)(0.0f);
	return (float4)(hit[0].normal, hit[0].dist);
}



bool intersect_scene(__constant t_object *objects, const int num_objects, const t_ray *ray,
						float3 *normal, float *t, int *object_id,
						bool quick)
{
	float inf = 1e20f; //initialise t to a very large number, so t will be guaranteed to be smaller	when a hit with the scene occurs
	float hitdistance = 0.0f;
	float3 loc_normal = (float3)(0.0f, 0.0f, 0.0f);
	*t = inf;

	for (int i = 0; i < num_objects; i++) //check if the ray intersects each object in the scene
	{
		t_object obj = objects[i]; // create local copy of object
		float4 res;
		switch(obj.type)
		{
			case PLANE :
				res = intersect_plane(&obj, ray, quick);
				hitdistance = res.w;
				loc_normal = res.xyz;
				break;
			case SPHERE :
				res = intersect_sphere(&obj, ray, quick);
				hitdistance = res.w;
				loc_normal = res.xyz;
				break;
			case CYLINDER :
				res = intersect_cylinder(&obj, ray, quick);
				hitdistance = res.w;
				loc_normal = res.xyz;
				break;
			case CONE :
				res = intersect_cone(&obj, ray, quick);
				hitdistance = res.w;
				loc_normal = res.xyz;
				break;
			case BOX :
				res = intersect_box(&obj, ray, quick);
				hitdistance = res.w;
				loc_normal = res.xyz;
				break;
		}
		if (hitdistance != 0.0f && hitdistance < *t) // keep track of the closest intersection and hitobject found so far
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
		if (!intersect_scene(spheres, sphere_count, &ray, &normal_facing, &t, &hitobject_id, false))
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
	if (!intersect_scene(objects, sphere_count, &ray, &normal, &t, &hitobject_id, false))
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
		ray.dir = fast_normalize(lights[i].loc - ray.origin);
		dist_to_light = fast_length(lights[i].loc - ray.origin);

		if (intersect_scene(objects, sphere_count, &ray, &dummy_normal, &t, &hitobject_id, true) && t < dist_to_light)
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
