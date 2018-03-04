#include <stdlib.h>
#include "rt.h"

static void	init_cam(t_scene *scene)
{
	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	scene->cam->pos = init_vec3(0.0f, 1.0f, 4.0f);
}

static void	init_lights(t_scene *scene)
{
	scene->num_lights = 2;
	scene->lights = (t_light*)malloc(sizeof(t_light) * scene->num_lights);
	scene->lights[0].pos = init_vec3(1.0f, 0.6f, 1.0f);
	scene->lights[0].emi = init_vec3(0.7f, 0.7f, 0.7f);
	scene->lights[1].pos = init_vec3(3.0f, 5.0f, -7.0f);
	scene->lights[1].emi = init_vec3(0.8f, 0.8f, 0.8f);
}

void	init_scene1(t_scene *scene)
{
	init_cam(scene);
	init_lights(scene);

	scene->num_objects = 2;
	scene->objects = (t_object*)malloc(sizeof(t_object) * scene->num_objects);
	// sphere
	scene->objects[0].type = SPHERE;
	scene->objects[0].radius = 0.9f;
	scene->objects[0].pos = init_vec3(0.0f, -0.2f, 0.2f);
	scene->objects[0].color = init_vec3(1.0f, 0.41f, 1.0f);
	scene->objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[0].diffuse = 0.8f;
	scene->objects[0].specular = 0.8f;
	scene->objects[0].spec_exp = 20.0f;

	// floor
	scene->objects[1].type = PLANE;
	scene->objects[1].pos = init_vec3(0.0f, -1.0f, 0.0f);
	scene->objects[1].dir = init_norm_vec3(0.0f, 1.0f, 0.0);
	scene->objects[1].color = init_vec3(0.9f, 0.8f, 0.7f);
	scene->objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].diffuse = 1.0f;
	scene->objects[1].specular = 0.0f;
	scene->objects[1].spec_exp = 0.5f;
}
