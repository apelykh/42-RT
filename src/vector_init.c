/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_init.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 20:20:47 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 20:21:08 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

cl_float3			init_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (res);
}

cl_float4			init_vec4(cl_float x, cl_float y, cl_float z, cl_float w)
{
	cl_float4 res;

	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	return (res);
}

cl_float3			init_norm_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (vec_norm(res));
}

cl_int2				init_int2(cl_int x, cl_int y)
{
	cl_int2	res;

	res.x = x;
	res.y = y;
	return (res);
}
