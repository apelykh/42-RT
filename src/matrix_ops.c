/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 19:55:01 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 16:38:01 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

cl_float4		mat_mult_vec(t_mat4 m, cl_float4 v)
{
	cl_float4 r;

	r.x = m.s0 * v.x + m.s4 * v.y + m.s8 * v.z + m.sC * v.w;
	r.y = m.s1 * v.x + m.s5 * v.y + m.s9 * v.z + m.sD * v.w;
	r.z = m.s2 * v.x + m.s6 * v.y + m.sA * v.z + m.sE * v.w;
	r.w = m.s3 * v.x + m.s7 * v.y + m.sB * v.z + m.sF * v.w;
	return (r);
}

t_mat4			mat_mult_mat(t_mat4 a, t_mat4 b)
{
	t_mat4 c;

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
	return (c);
}

static t_mat4	mat_invert_(t_mat4 m, cl_float det)
{
	t_mat4		r;
	cl_float4	b;

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

t_mat4			mat_invert(t_mat4 m)
{
	t_mat4		r;
	cl_float	det;

	det = m.s0 * m.s5 * m.sA + m.s1 * m.s6 * m.s8 + m.s2 * m.s4 * m.s9 - m.s0 *
			m.s6 * m.s9 - m.s2 * m.s5 * m.s8 - m.s1 * m.s4 * m.sA;
	if (det != 0.0f)
	{
		det = 1.0f / det;
		r = mat_invert_(m, det);
	}
	else
		r = m;
	return (r);
}

t_mat4			mat_transpose(t_mat4 m)
{
	t_mat4 r;

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
	return (r);
}
