/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix_ops2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 20:01:46 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 20:03:05 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

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
	return (r);
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
	return (r);
}

mat4	mat_rotatex(cl_float angle)
{
	mat4		r;
	cl_float	sint;
	cl_float	cost;

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
	return (r);
}

mat4	mat_rotatey(cl_float angle)
{
	mat4		r;
	cl_float	sint;
	cl_float	cost;

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
	return (r);
}

mat4	mat_rotatez(cl_float angle)
{
	mat4		r;
	cl_float	sint;
	cl_float	cost;

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
	return (r);
}
