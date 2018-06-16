/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 20:14:31 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 20:27:58 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static cl_float	vec_dot(cl_float3 a, cl_float3 b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

cl_float3		vec_norm(cl_float3 a)
{
	cl_float	len;

	len = sqrt(vec_dot(a, a));
	if (len != 0)
	{
		a.x /= len;
		a.y /= len;
		a.z /= len;
	}
	return (a);
}

cl_float3		add_vec3(cl_float3 a, cl_float3 b)
{
	return (init_vec3(a.x + b.x, a.y + b.y, a.z + b.z));
}
