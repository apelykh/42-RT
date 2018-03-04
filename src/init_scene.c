#include "rt.h"

cl_float3	init_vec3(cl_float x, cl_float y, cl_float z)
{
	cl_float3	res;

	res.x = x;
	res.y = y;
	res.z = z;
	return (res);
}

void	init_objects(t_object *cpu_objects)
{
	// left wall
	// cpu_objects[0].type = 0;
	// cpu_objects[0].pos = init_vec3(-0.6f, 0.0f, 0.0f);
	// cpu_objects[0].dir = init_vec3(1.0, 0.0, 0.0);
	// cpu_objects[0].color = init_vec3(0.75f, 0.25f, 0.25f);
	// cpu_objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_objects[0].diffuse = 1.0f;
	// cpu_objects[0].specular = 0.5f;
	// cpu_objects[0].spec_exp = 0.5f;

	// right wall
	// cpu_objects[1].type = 0;
	// cpu_objects[1].pos = init_vec3(0.6f, 0.0f, 0.0f);
	// cpu_objects[1].dir = init_vec3(-1.0, 0.0, 0.0);
	// cpu_objects[1].color = init_vec3(0.25f, 0.25f, 0.75f);
	// cpu_objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_objects[1].diffuse = 1.0f;
	// cpu_objects[1].specular = 0.5f;
	// cpu_objects[1].spec_exp = 0.5f;

	// floor
	cpu_objects[0].type = 0;
	cpu_objects[0].pos = init_vec3(0.0f, -0.4f, 0.0f);
	cpu_objects[0].dir = init_vec3(0.0f, 1.0f, 0.0);
	cpu_objects[0].color = init_vec3(0.9f, 0.8f, 0.7f);
	cpu_objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[0].diffuse = 1.0f;
	cpu_objects[0].specular = 0.5f;
	cpu_objects[0].spec_exp = 0.5f;

	// ceiling
	// cpu_objects[3].type = 0;
	// cpu_objects[3].pos = init_vec3(0.0f, 0.4f, 0.0f);
	// cpu_objects[3].dir = init_vec3(0.0f, -1.0f, 0.0);
	// cpu_objects[3].color = init_vec3(0.9f, 0.8f, 0.7f);
	// cpu_objects[3].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_objects[3].diffuse = 1.0f;
	// cpu_objects[3].specular = 0.5f;
	// cpu_objects[3].spec_exp = 0.5f;

	// back wall
	// cpu_objects[4].type = 0;
	// cpu_objects[4].pos = init_vec3(0.0f, 0.0f, -0.4f);
	// cpu_objects[4].dir = init_vec3(0.0f, 0.0f, -1.0f);
	// cpu_objects[4].color = init_vec3(0.9f, 0.8f, 0.7f);
	// cpu_objects[4].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_objects[4].diffuse = 1.0f;
	// cpu_objects[4].specular = 0.5f;
	// cpu_objects[4].spec_exp = 0.5f;

	//left cone
	cpu_objects[1].type = 3;
	cpu_objects[1].k = 30.0f;
	cpu_objects[1].pos = init_vec3(-0.25f, 0.2f, 0.25f);
	cpu_objects[1].dir = init_vec3(-1.0f, 0.0f, 0.0f);
	cpu_objects[1].color = init_vec3(1.0f, 0.0f, 0.0f);
	cpu_objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[1].diffuse = 1.0f;
	cpu_objects[1].specular = 0.0f;
	cpu_objects[1].spec_exp = 0.5f;

	// sphere
	cpu_objects[2].type = 1;
	cpu_objects[2].radius = 0.5f;
	cpu_objects[2].pos = init_vec3(0.0f, -0.5f, 1.0f);
	cpu_objects[2].color = init_vec3(0.5f, 0.6f, 0.7f);
	cpu_objects[2].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[2].diffuse = 0.5f;
	cpu_objects[2].specular = 0.5f;
	cpu_objects[2].spec_exp = 20.0f;

	// right cylinder
	cpu_objects[3].type = 2;
	cpu_objects[3].radius = 0.5f;
	cpu_objects[3].pos = init_vec3(0.25f, -0.35f, 0.1f);
	cpu_objects[3].dir = init_vec3(0.0f, 1.0f, 0.0f);
	cpu_objects[3].color = init_vec3(0.5f, 0.6f, 0.7f);
	cpu_objects[3].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[3].diffuse = 1.0f;
	cpu_objects[3].specular = 0.0f;
	cpu_objects[3].spec_exp = 0.5f;

	// dome
	cpu_objects[4].type = 1;
	cpu_objects[4].radius = 20.0f;
	cpu_objects[4].pos = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[4].color = init_vec3(0.75f, 0.25f, 0.75f);
	cpu_objects[4].emi = init_vec3(0.0f, 0.0f, 0.0f);
	cpu_objects[4].diffuse = 1.0f;

	// lightsource
	// cpu_objects[7].type = 1;
	// cpu_objects[7].radius = 0.05f;
	// cpu_objects[7].pos = init_vec3(0.0f, 0.6f, 0.5f);
	// cpu_objects[7].color = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_objects[7].emi = init_vec3(27.0f, 30.0f, 33.0f);
	// cpu_objects[7].diffuse = 1.0f;
	// cpu_objects[7].specular = 0.5f;
	// cpu_objects[7].spec_exp = 0.5f;

	// -----------------------------------------------------------

	// // left wall
	// cpu_spheres[0].radius = 200.0f;
	// cpu_spheres[0].pos = init_vec3(-200.6f, 0.0f, 0.0f);
	// cpu_spheres[0].color = init_vec3(0.75f, 0.25f, 0.25f);
	// cpu_spheres[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[0].diffuse = 1.0f;

	// // right wall
	// cpu_spheres[1].radius = 200.0f;
	// cpu_spheres[1].pos = init_vec3(200.6f, 0.0f, 0.0f);
	// cpu_spheres[1].color = init_vec3(0.25f, 0.25f, 0.75f);
	// cpu_spheres[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[1].diffuse = 1.0f;

	// // floor
	// cpu_spheres[0].radius = 200.0f;
	// cpu_spheres[0].pos = init_vec3(0.0f, -200.4f, 0.0f);
	// cpu_spheres[0].color = init_vec3(0.9f, 0.8f, 0.7f);
	// cpu_spheres[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[0].diffuse = 1.0f;
	// cpu_spheres[0].specular = 0.0f;
	// cpu_spheres[0].spec_exp = 0.5f;


	// // back wall
	// cpu_spheres[3].radius = 200.0f;
	// cpu_spheres[3].pos = init_vec3(0.0f, 0.0f, -200.4f);
	// cpu_spheres[3].color = init_vec3(0.8f, 0.7f, 0.6f);
	// cpu_spheres[3].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[3].diffuse = 1.0f;


	// // left sphere
	// cpu_spheres[1].radius = 0.16f;
	// cpu_spheres[1].pos = init_vec3(0.0f, 0.0f, -0.7f);
	// cpu_spheres[1].color = init_vec3(0.5f, 0.6f, 0.7f);
	// cpu_spheres[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[1].diffuse = 0.5f;
	// cpu_spheres[1].specular = 0.5f;
	// cpu_spheres[1].spec_exp = 20.0f;

	// // right sphere
	// cpu_spheres[5].radius = 1.6f;
	// cpu_spheres[5].pos = init_vec3(0.0f, 0.0f, -1.0f);
	// cpu_spheres[5].color = init_vec3(0.7f, 0.6f, 0.7f);
	// cpu_spheres[5].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// cpu_spheres[5].diffuse = 0.5f;
	// cpu_spheres[5].specular = 0.5f;
	// cpu_spheres[5].spec_exp = 20.0f;

	// // lightsource
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
