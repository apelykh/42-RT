#include "rt.h"

cl_float3	init_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (res);
}

void	init_objects(t_object *cpu_spheres)
{
	// left wall
	// cpu_spheres[0].radius = 200.0f;
	// cpu_spheres[0].pos.x = -200.6f;
	// cpu_spheres[0].pos.y = 0.0f;
	// cpu_spheres[0].pos.z = 0.0f;
	// cpu_spheres[0].color.x = 0.75f;
	// cpu_spheres[0].color.y = 0.25f;
	// cpu_spheres[0].color.z = 0.25f;
	// cpu_spheres[0].emi.x = 0.1f;
	// cpu_spheres[0].emi.y = 0.2f;
	// cpu_spheres[0].emi.z = 0.3f;

	// right wall
	// cpu_spheres[1].radius = 200.0f;
	// cpu_spheres[1].pos.x = 200.6f;
	// cpu_spheres[1].pos.y = 0.0f;
	// cpu_spheres[1].pos.z = 0.0f;
	// cpu_spheres[1].color.x = 0.25f;
	// cpu_spheres[1].color.y = 0.25f;
	// cpu_spheres[1].color.z = 0.75f;
	// cpu_spheres[1].emi.x = 0.1f;
	// cpu_spheres[1].emi.y = 0.2f;
	// cpu_spheres[1].emi.z = 0.3f;

	// floor
	cpu_spheres[0].radius = 200.0f;
	cpu_spheres[0].pos = init_vec3(0.0f, -200.4f, 0.0f);
	cpu_spheres[0].color = init_vec3(0.9f, 0.8f, 0.7f);
	cpu_spheres[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_spheres[0].diffuse = 1.0f;
	cpu_spheres[0].specular = 0.5f;
	cpu_spheres[0].spec_exp = 0.5f;

	// ceiling
	// cpu_spheres[3].radius = 200.0f;
	// cpu_spheres[3].pos.x = 0.0f;
	// cpu_spheres[3].pos.y = 200.4f;
	// cpu_spheres[3].pos.z = 0.0f;
	// cpu_spheres[3].color.x = 0.9f;
	// cpu_spheres[3].color.y = 0.8f;
	// cpu_spheres[3].color.z = 0.7f;
	// cpu_spheres[3].emi.x = 0.0f;
	// cpu_spheres[3].emi.y = 0.0f;
	// cpu_spheres[3].emi.z = 0.0f;

	// back wall
	// cpu_spheres[4].radius = 200.0f;
	// cpu_spheres[4].pos.x = 0.0f;
	// cpu_spheres[4].pos.y = 0.0f;
	// cpu_spheres[4].pos.z = -200.4f;
	// cpu_spheres[4].color.x = 0.9f;
	// cpu_spheres[4].color.y = 0.8f;
	// cpu_spheres[4].color.z = 0.7f;
	// cpu_spheres[4].emi.x = 0.0f;
	// cpu_spheres[4].emi.y = 0.0f;
	// cpu_spheres[4].emi.z = 0.0f;

	// // front wall 
	// cpu_spheres[5].radius = 200.0f;
	// cpu_spheres[5].pos.x = 0.0f;
	// cpu_spheres[5].pos.y = 0.0f;
	// cpu_spheres[5].pos.z = 202.0f;
	// cpu_spheres[5].color.x = 0.9f;
	// cpu_spheres[5].color.y = 0.8f;
	// cpu_spheres[5].color.z = 0.7f;
	// cpu_spheres[5].emi.x = 0.0f;
	// cpu_spheres[5].emi.y = 0.0f;
	// cpu_spheres[5].emi.z = 0.0f;

	// left sphere
	cpu_spheres[1].radius = 0.16f;
	cpu_spheres[1].pos = init_vec3(-0.3f, -0.24f, -0.1f);
	cpu_spheres[1].color = init_vec3(0.5f, 0.6f, 0.7f);
	cpu_spheres[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_spheres[1].diffuse = 0.5f;
	cpu_spheres[1].specular = 0.5f;
	cpu_spheres[1].spec_exp = 20.0f;

	// right sphere
	cpu_spheres[2].radius = 0.16f;
	cpu_spheres[2].pos = init_vec3(0.3f, -0.3f, -0.1f);
	cpu_spheres[2].color = init_vec3(0.7f, 0.6f, 0.7f);
	cpu_spheres[2].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_spheres[2].diffuse = 0.5f;
	cpu_spheres[2].specular = 0.5f;
	cpu_spheres[2].spec_exp = 20.0f;

	// lightsource
	// cpu_spheres[7].radius = 0.05f;
	// cpu_spheres[7].pos.x = 0.0f;
	// cpu_spheres[7].pos.y = 0.3f;
	// cpu_spheres[7].pos.z = 0.5f;
	// cpu_spheres[7].color.x = 0.7f;
	// cpu_spheres[7].color.y = 0.2f;
	// cpu_spheres[7].color.z = 0.4f;
	// cpu_spheres[7].emi.x = 0.75f;
	// cpu_spheres[7].emi.y = 0.75f;
	// cpu_spheres[7].emi.z = 0.75f;
}
