#include <stdlib.h>
#include "rt.h"

static void	init_cam(t_scene *scene)
{
	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	scene->cam->pos = init_vec3(0.0f, 0.5f, 2.5f);
}

static void	init_lights(t_scene *scene)
{
	scene->num_lights = 1;
	scene->lights = (t_light*)malloc(sizeof(t_light) * scene->num_lights);
	scene->lights[0].pos = init_vec3(0.7f, 0.5f, 4.0f);
	scene->lights[0].emi = init_vec3(0.7f, 0.7f, 0.7f);
	// scene->lights[1].pos = init_vec3(3.0f, 5.0f, -7.0f);
	// scene->lights[1].emi = init_vec3(0.8f, 0.8f, 0.8f);
}

void	init_scene2(t_scene *scene)
{
	init_cam(scene);
	init_lights(scene);

	scene->num_objects = 6;
	scene->objects = (t_object*)malloc(sizeof(t_object) * scene->num_objects);

	// left wall
	scene->objects[0].type = PLANE;
	scene->objects[0].pos = init_vec3(-2.0f, 0.0f, 0.0f);
	scene->objects[0].dir = init_norm_vec3(1.0, 0.0, 0.0);
	scene->objects[0].color = init_vec3(0.75f, 0.25f, 0.25f);
	scene->objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[0].diffuse = 1.0f;
	scene->objects[0].specular = 0.0f;
	scene->objects[0].spec_exp = 0.5f;

	// right wall
	scene->objects[1].type = PLANE;
	scene->objects[1].pos = init_vec3(2.0f, 0.0f, 0.0f);
	scene->objects[1].dir = init_norm_vec3(-1.0, 0.0, 0.0);
	scene->objects[1].color = init_vec3(0.25f, 0.25f, 0.75f);
	scene->objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].diffuse = 1.0f;
	scene->objects[1].specular = 0.0f;
	scene->objects[1].spec_exp = 0.5f;

	// floor
	scene->objects[2].type = PLANE;
	scene->objects[2].pos = init_vec3(0.0f, -0.4f, 0.0f);
	scene->objects[2].dir = init_vec3(0.0f, 1.0f, 0.0);
	scene->objects[2].color = init_vec3(0.9f, 0.8f, 0.7f);
	scene->objects[2].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[2].diffuse = 1.0f;
	scene->objects[2].specular = 0.5f;
	scene->objects[2].spec_exp = 0.5f;

	//left cone
	scene->objects[3].type = CONE;
	scene->objects[3].k = 30.0f;
	scene->objects[3].pos = init_vec3(-0.25f, 0.7f, 0.25f);
	scene->objects[3].dir = init_norm_vec3(1.0f, 1.0f, 0.0f);
	scene->objects[3].color = init_vec3(1.0f, 0.0f, 0.0f);
	scene->objects[3].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[3].diffuse = 1.0f;
	scene->objects[3].specular = 0.0f;
	scene->objects[3].spec_exp = 0.5f;

	// sphere
	scene->objects[4].type = SPHERE;
	scene->objects[4].radius = 0.6f;
	scene->objects[4].pos = init_vec3(-0.5f, -0.2f, 0.2f);
	scene->objects[4].color = init_vec3(1.0f, 0.41f, 1.0f);
	scene->objects[4].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[4].diffuse = 0.8f;
	scene->objects[4].specular = 0.8f;
	scene->objects[4].spec_exp = 20.0f;

	// right cylinder
	scene->objects[5].type = CYLINDER;
	scene->objects[5].radius = 0.5f;
	scene->objects[5].pos = init_vec3(0.3f, -0.4f, 0.1f);
	scene->objects[5].dir = init_norm_vec3(0.0f, 1.0f, 0.0f);
	scene->objects[5].color = init_vec3(0.5f, 0.6f, 0.7f);
	scene->objects[5].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[5].diffuse = 1.0f;
	scene->objects[5].specular = 0.0f;
	scene->objects[5].spec_exp = 0.5f;
}
