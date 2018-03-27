#include "rt.h"

void	obj_transform_mats(t_object *obj)
{
	mat4 tra;
	mat4 rot;
	mat4 sca;

	tra = mat_translate(obj->loc);
	rot = mat_rotx(obj->rot.x);
	rot = mat_mult_mat(mat_roty(obj->rot.y), rot);
	rot = mat_mult_mat(mat_rotz(obj->rot.z), rot);
	sca = mat_scale(obj->sca);
	obj->from_local = mat_mult_mat(mat_mult_mat(tra, rot), sca);
	obj->to_local = mat_invert(obj->from_local);
}

mat4	mat_translate(cl_float3 v)
{
	mat4 r;

	r.s0 = 1.0f;
	r.s1 = 0.0f;
	r.s2 = 0.0f;
	r.s3 = 0.0f;
	r.s4 = 0.0f;
	r.s5 = 1.0f;
	r.s6 = 0.0f;
	r.s7 = 0.0f;
	r.s8 = 0.0f;
	r.s9 = 0.0f;
	r.sA = 1.0f;
	r.sB = 0.0f;
	r.sC = v.x;
	r.sD = v.y;
	r.sE = v.z;
	r.sF = 1.0f;
	return r;
}

mat4	mat_scale(cl_float3 s)
{
	mat4 r;

	r.s0 = s.x;
	r.s1 = 0.0f;
	r.s2 = 0.0f;
	r.s3 = 0.0f;
	r.s4 = 0.0f;
	r.s5 = s.y;
	r.s6 = 0.0f;
	r.s7 = 0.0f;
	r.s8 = 0.0f;
	r.s9 = 0.0f;
	r.sA = s.z;
	r.sB = 0.0f;
	r.sC = 0.0f;
	r.sD = 0.0f;
	r.sE = 0.0f;
	r.sF = 1.0f;
	return r;
}

mat4 mat_rotx(cl_float angle)
{	
	mat4 r;
	cl_float sint;
	cl_float cost;
	
	sint = sin(angle * M_PI / 180);
	cost = cos(angle * M_PI / 180);
	r.s0 = 1.0f;
	r.s1 = 0.0f;
	r.s2 = 0.0f;
	r.s3 = 0.0f;
	r.s4 = 0.0f;
	r.s5 = cost;
	r.s6 = sint;
	r.s7 = 0.0f;
	r.s8 = 0.0f;
	r.s9 = -sint;
	r.sA = cost;
	r.sB = 0.0f;
	r.sC = 0.0f;
	r.sD = 0.0f;
	r.sE = 0.0f;
	r.sF = 1.0f;
	return r;
}

mat4 mat_roty(cl_float angle)
{
	mat4 r;
	cl_float sint;
	cl_float cost;
	
	sint = sin(angle * M_PI / 180);
	cost = cos(angle * M_PI / 180);
	r.s0 = cost;
	r.s1 = 0.0f;
	r.s2 = -sint;
	r.s3 = 0.0f;
	r.s4 = 0.0f;
	r.s5 = 1.0f;
	r.s6 = 0.0f;
	r.s7 = 0.0f;
	r.s8 = sint;
	r.s9 = 0.0f;
	r.sA = cost;
	r.sB = 0.0f;
	r.sC = 0.0f;
	r.sD = 0.0f;
	r.sE = 0.0f;
	r.sF = 1.0f;
	return r;
}

mat4 mat_rotz(cl_float angle)
{
	mat4 r;
	cl_float sint;
	cl_float cost;
	
	sint = sin(angle * M_PI / 180);
	cost = cos(angle * M_PI / 180);
	r.s0 = cost;
	r.s1 = sint;
	r.s2 = 0.0f;
	r.s3 = 0.0f;
	r.s4 = -sint;
	r.s5 = cost;
	r.s6 = 0.0f;
	r.s7 = 0.0f;
	r.s8 = 0.0f;
	r.s9 = 0.0f;
	r.sA = 1.0f;
	r.sB = 0.0f;
	r.sC = 0.0f;
	r.sD = 0.0f;
	r.sE = 0.0f;
	r.sF = 1.0f;
	return r;
}

cl_float4 mat_mult_vec(mat4 m, cl_float4 v)
{
	cl_float4 r;

	r.x = m.s0 * v.x + m.s4 * v.y + m.s8 * v.z + m.sC * v.w;
	r.y = m.s1 * v.x + m.s5 * v.y + m.s9 * v.z + m.sD * v.w;
	r.z = m.s2 * v.x + m.s6 * v.y + m.sA * v.z + m.sE * v.w;
	r.w = m.s3 * v.x + m.s7 * v.y + m.sB * v.z + m.sF * v.w;
	return (r);
}

mat4 mat_invert_(mat4 m, cl_float det)
{
	cl_float4 b;
	mat4 r;

	r.s0 = (m.s5 * m.sA - m.s9 * m.s6) * det;
	r.s1 = (m.s9 * m.s2 - m.s1 * m.sA) * det;
	r.s2 = (m.s1 * m.s6 - m.s5 * m.s2) * det;
	r.s3 = 0.0f;
	r.s4 = (m.s8 * m.s6 - m.s4 * m.sA) * det;
	r.s5 = (m.s0 * m.sA - m.s8 * m.s2) * det;
	r.s6 = (m.s4 * m.s2 - m.s0 * m.s6) * det;
	r.s7 = 0.0f;
	r.s8 = (m.s4 * m.s9 - m.s8 * m.s5) * det;
	r.s9 = (m.s8 * m.s1 - m.s0 * m.s9) * det;
	r.sA = (m.s0 * m.s5 - m.s4 * m.s1) * det;
	r.sB = 0.0f;
	r.sC = 0.0f;
	r.sD = 0.0f;
	r.sE = 0.0f;
	r.sF = 1.0f;
	b = mat_mult_vec(r, init_vec4(-m.sC, -m.sD, -m.sE, -m.sF));
	r.sC = b.x;
	r.sD = b.y;
	r.sE = b.z;
	return (r);
}


mat4 mat_invert(mat4 m)
{
	mat4 r;
	cl_float det;

	det = m.s0 * m.s5 * m.sA + m.s1 * m.s6 * m.s8 + m.s2 * m.s4 * m.s9 - m.s0 *
			m.s6 * m.s9 - m.s2 * m.s5 * m.s8 - m.s1* m.s4 * m.sA;
	if (det != 0.0f)
	{
		det = 1.0f / det;
		r = mat_invert_(m, det);
	}
	else
		r = m;
	return r;
}

mat4 mat_transpose(mat4 m)
{
	mat4 r;

	r.s0 = m.s0;
	r.s1 = m.s4;
	r.s2 = m.s8;
	r.s3 = m.sC;
	r.s4 = m.s1;
	r.s5 = m.s5;
	r.s6 = m.s9;
	r.s7 = m.sD;
	r.s8 = m.s2;
	r.s9 = m.s6;
	r.sA = m.sA;
	r.sB = m.sE;
	r.sC = m.s3;
	r.sD = m.s7;
	r.sE = m.sB;
	r.sF = m.sF;
	return r;
}

mat4 mat_mult_mat(mat4 a, mat4 b)
{
	mat4 c;

	c.s0 = a.s0 * b.s0 + a.s4 * b.s1 + a.s8 * b.s2 + a.sC * b.s3;
	c.s1 = a.s1 * b.s0 + a.s5 * b.s1 + a.s9 * b.s2 + a.sD * b.s3;
	c.s2 = a.s2 * b.s0 + a.s6 * b.s1 + a.sA * b.s2 + a.sE * b.s3;
	c.s3 = a.s3 * b.s0 + a.s7 * b.s1 + a.sB * b.s2 + a.sF * b.s3;
	c.s4 = a.s0 * b.s4 + a.s4 * b.s5 + a.s8 * b.s6 + a.sC * b.s7;
	c.s5 = a.s1 * b.s4 + a.s5 * b.s5 + a.s9 * b.s6 + a.sD * b.s7;
	c.s6 = a.s2 * b.s4 + a.s6 * b.s5 + a.sA * b.s6 + a.sE * b.s7;
	c.s7 = a.s3 * b.s4 + a.s7 * b.s5 + a.sB * b.s6 + a.sF * b.s7;
	c.s8 = a.s0 * b.s8 + a.s4 * b.s9 + a.s8 * b.sA + a.sC * b.sB;
	c.s9 = a.s1 * b.s8 + a.s5 * b.s9 + a.s9 * b.sA + a.sD * b.sB;
	c.sA = a.s2 * b.s8 + a.s6 * b.s9 + a.sA * b.sA + a.sE * b.sB;
	c.sB = a.s3 * b.s8 + a.s7 * b.s9 + a.sB * b.sA + a.sF * b.sB;
	c.sC = a.s0 * b.sC + a.s4 * b.sD + a.s8 * b.sE + a.sC * b.sF;
	c.sD = a.s1 * b.sC + a.s5 * b.sD + a.s9 * b.sE + a.sD * b.sF;
	c.sE = a.s2 * b.sC + a.s6 * b.sD + a.sA * b.sE + a.sE * b.sF;
	c.sF = a.s3 * b.sC + a.s7 * b.sD + a.sB * b.sE + a.sF * b.sF;
	return c;
}