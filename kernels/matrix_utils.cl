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
