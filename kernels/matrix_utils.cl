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

mat4	mat_rotatex(float angle)
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

mat4	mat_rotatey(float angle)
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

mat4	mat_rotatez(float angle)
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

	float det = m.s0 * m.s5 * m.sA + m.s1 * m.s6 * m.s8 + m.s2 * m.s4 * m.s9 -
		m.s0 * m.s6 * m.s9 - m.s2 * m.s5 * m.s8 - m.s1* m.s4 * m.sA;

	if (det != 0.0f)
	{
		det = 1.f / det;
		r.s0123 = (float4)((m.s5 * m.sA - m.s9 * m.s6) * det,
			(m.s9 * m.s2 - m.s1 * m.sA) * det, (m.s1 * m.s6 - m.s5 * m.s2) * det, 0.0f);
		r.s4567 = (float4)((m.s8 * m.s6 - m.s4 * m.sA) * det,
			(m.s0 * m.sA - m.s8 * m.s2) * det, (m.s4 * m.s2 - m.s0 * m.s6) * det, 0.0f);
		r.s89AB = (float4)((m.s4 * m.s9 - m.s8 * m.s5) * det,
			(m.s8 * m.s1 - m.s0 * m.s9) * det, (m.s0 * m.s5 - m.s4 * m.s1) * det, 0.0f);
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

	c.s0123 = (float4)(dot(a.s048C, b.s0123), dot(a.s159D, b.s0123),
		dot(a.s26AE, b.s0123), dot(a.s37BF, b.s0123));
	c.s4567 = (float4)(dot(a.s048C, b.s4567), dot(a.s159D, b.s4567),
		dot(a.s26AE, b.s4567), dot(a.s37BF, b.s4567));
	c.s89AB = (float4)(dot(a.s048C, b.s89AB), dot(a.s159D, b.s89AB),
		dot(a.s26AE, b.s89AB), dot(a.s37BF, b.s89AB));
	c.sCDEF = (float4)(dot(a.s048C, b.sCDEF), dot(a.s159D, b.sCDEF),
		dot(a.s26AE, b.sCDEF), dot(a.s37BF, b.sCDEF));
	return c;
}

float4 mat_mult_vec(mat4 m, float4 v)
{
	return ((float4)(dot(m.s048C, v), dot(m.s159D, v),
		dot(m.s26AE, v), dot(m.s37BF, v)));
}

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
