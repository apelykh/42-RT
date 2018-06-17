t_mat4 mat_invert(t_mat4 m)
{
	t_mat4 r;

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

t_mat4 mat_transpose(t_mat4 m)
{
	t_mat4 r;

	r.s0123 = m.s048C;
	r.s4567 = m.s159D;
	r.s89AB = m.s26AE;
	r.sCDEF	= m.s37BF;
	return r;
}

t_mat4 mat_mult_mat(t_mat4 a, t_mat4 b)
{
	t_mat4 c;

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

float4 mat_mult_vec(t_mat4 m, float4 v)
{
	return ((float4)(dot(m.s048C, v), dot(m.s159D, v),
		dot(m.s26AE, v), dot(m.s37BF, v)));
}

t_mat4	mat_transform(const t_object *obj)
{
	t_mat4 translate, rotate, scale = (float16)(0.f);
	translate = mat_translate(obj->location);
	rotate = mat_rotatex(obj->rotation.x);
	rotate = mat_mult_mat(mat_rotatey(obj->rotation.y), rotate);
	rotate = mat_mult_mat(mat_rotatez(obj->rotation.z), rotate);
	scale = mat_scale(obj->scale);
	return (mat_mult_mat(mat_mult_mat(translate, rotate), scale));
}
