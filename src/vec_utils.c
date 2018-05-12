#include "rt.h"

static cl_float		vec_dot(cl_float3 a, cl_float3 b)
{
	return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

static cl_float3	vec_norm(cl_float3 a)
{
	cl_float    len;

	len = sqrt(vec_dot(a, a));
	if (len != 0)
	{
		a.x /= len;
		a.y /= len;
		a.z /= len;
	}
	return (a);
}

cl_float3	add_vec3(cl_float3 a, cl_float3 b)
{
	return init_vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

cl_int2		init_int2(cl_int x, cl_int y)
{
	cl_int2	res;

	res.x = x;
	res.y = y;
	return (res);
}

cl_float3	init_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (res);
}

cl_float4	init_vec4(cl_float x, cl_float y, cl_float z, cl_float w)
{
	cl_float4 res;

	res.x = x;
	res.y = y;
	res.z = z;
	res.w = w;
	return (res);
}

cl_float3	init_norm_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (vec_norm(res));
}
