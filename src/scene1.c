#include <stdlib.h>
#include "rt.h"

static void	init_cam(t_scene *scene)
{
	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	scene->cam->loc = init_vec3(0.0f, 1.0f, 10.f);
	scene->cam->rot = init_vec3(0.f, 0.f, 45.f);
	scene->cam->fov = 45.f;
}

static void	init_lights(t_scene *scene)
{	
	scene->num_lights = 2;
	scene->lights = (t_light*)malloc(sizeof(t_light) * scene->num_lights);
	scene->lights[0].loc = init_vec3(0.0f, 2.75f, 2.75f);
	scene->lights[0].emi = init_vec3(1.f, 1.f, 1.f);
	scene->lights[1].loc = init_vec3(3.0f, 5.0f, -7.0f);
	scene->lights[1].emi = init_vec3(0.5f, 0.5f, 0.5f);
}

// static void	init_lights(t_scene *scene)
// {
// 	scene->num_lights = 1;
// 	scene->lights = (t_light*)malloc(sizeof(t_light) * scene->num_lights);
// 	scene->lights[0].pos = init_vec3(0.0f, 2.75f, 2.75f);
// 	scene->lights[0].emi = init_vec3(1.0f, 1.0f, 1.0f);
// }

void	init_scene1(t_scene *scene)
{
	init_cam(scene);
	init_lights(scene);

	scene->num_objects = 9;
	scene->objects = (t_object*)malloc(sizeof(t_object) * scene->num_objects);
	

	scene->objects[0].id = 0;
	scene->objects[0].type = PLANE;
	scene->objects[0].sca = init_vec3(1.0f, 1.0f, 1.0f);
	scene->objects[0].rot = init_vec3(0.f, 0.f, 0.f);
	scene->objects[0].loc = init_vec3(0.0f, -1.0f, 0.0f);
	scene->objects[0].color = init_vec3(0.9f, 0.8f, 0.7f);
	scene->objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[0].diffuse = 1.0f;
	scene->objects[0].specular = 0.0f;
	scene->objects[0].spec_exp = 0.5f;

	// scene->objects[1].id = 1;
	// scene->objects[1].type = CONE;
	// scene->objects[1].sca = init_vec3(1.f, 3.f, 1.f);
	// scene->objects[1].rot = init_vec3(30.f, 0.f, 30.f);
	// scene->objects[1].loc = init_vec3(0.0f, 2.0f, -2.1f);
	// scene->objects[1].color = init_vec3(.9f, 0.2f, 0.5f);
	// scene->objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[1].capped = true;
	// scene->objects[1].diffuse = 0.7f;
	// scene->objects[1].specular = 0.9f;
	// scene->objects[1].spec_exp = 20.0f;

	scene->objects[1].id = 1;
	scene->objects[1].type = CONE;
	scene->objects[1].sca = init_vec3(.5f, 1.f, .5f);
	scene->objects[1].rot = init_vec3(-15.f, 0.f, -15.f);
	scene->objects[1].loc = init_vec3(0.0f, 2.0f, -2.0f);
	scene->objects[1].color = init_vec3(0.95f, 0.0f, 0.0f);
	scene->objects[1].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].capped = false;
	scene->objects[1].diffuse = 0.8f;
	scene->objects[1].specular = 0.8f;
	scene->objects[1].spec_exp = 20.0f;

	scene->objects[2].id = 2;
	scene->objects[2].type = SPHERE;
	scene->objects[2].sca = init_vec3(5.f, .5f, 1.f);
	scene->objects[2].rot = init_vec3(45.f, 0.f, 0.f);
	scene->objects[2].loc = init_vec3(2.5f, 1.0f, -2.0f);
	scene->objects[2].color = init_vec3(0.98f, 0.68f, .82f);
	scene->objects[2].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[2].diffuse = 0.7f;
	scene->objects[2].specular = 0.9f;
	scene->objects[2].spec_exp = 20.0f;

	scene->objects[3].id = 3;
	scene->objects[3].type = SPHERE;
	scene->objects[3].sca = init_vec3(2.f, 2.f, 2.f);
	scene->objects[3].rot = init_vec3(0.f, 0.f, 0.f);
	scene->objects[3].loc = init_vec3(2.5f, 1.0f, -2.0f);
	scene->objects[3].color = init_vec3(0.98f, 0.68f, .82f);
	scene->objects[3].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[3].diffuse = 0.9f;
	scene->objects[3].specular = 0.9f;
	scene->objects[3].spec_exp = 20.0f;

	scene->objects[4].id = 4;
	scene->objects[4].type = BOX;
	scene->objects[4].sca = init_vec3(2.f, 3.f, .05f);
	scene->objects[4].rot = init_vec3(0.f, 60.f, -30.f);
	scene->objects[4].loc = init_vec3(-2.5f, 1.0f, -2.0f);
	scene->objects[4].color = init_vec3(1.0f, 0.49f, 0.0f);
	scene->objects[4].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[4].diffuse = 0.9f;
	scene->objects[4].specular = 0.9f;
	scene->objects[4].spec_exp = 20.0f;

	scene->objects[5].id = 5;
	scene->objects[5].type = CYLINDER;
	scene->objects[5].sca = init_vec3(.1f, 8.f, .1f);
	scene->objects[5].rot = init_vec3(0.f, 0.f, 90.f);
	scene->objects[5].loc = init_vec3(0.0f, 2.0f, -2.1f);
	scene->objects[5].color = init_vec3(.3f, 0.3f, 0.3f);
	scene->objects[5].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[5].capped = false;
	scene->objects[5].diffuse = 0.7f;
	scene->objects[5].specular = 0.9f;
	scene->objects[5].spec_exp = 20.0f;

	scene->objects[6].id = 6;
	scene->objects[6].type = SPHERE;
	scene->objects[6].sca = init_vec3(.3f, .25f, .3f);
	scene->objects[6].rot = init_vec3(0.f, 0.f, 0.f);
	scene->objects[6].loc = init_vec3(2.f, 1.5f, -1.1f);
	scene->objects[6].color = init_vec3(0.1f, 0.1f, .1f);
	scene->objects[6].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[6].diffuse = 0.9f;
	scene->objects[6].specular = 0.9f;
	scene->objects[6].spec_exp = 20.0f;

	scene->objects[7].id = 7;
	scene->objects[7].type = SPHERE;
	scene->objects[7].sca = init_vec3(.3f, .25f, .3f);
	scene->objects[7].rot = init_vec3(0.f, 0.f, 0.f);
	scene->objects[7].loc = init_vec3(2.6f, 1.5f, -1.1f);
	scene->objects[7].color = init_vec3(0.1f, 0.1f, .1f);
	scene->objects[7].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[7].diffuse = 0.9f;
	scene->objects[7].specular = 0.9f;
	scene->objects[7].spec_exp = 20.0f;

	scene->objects[8].id = 8;
	scene->objects[8].type = SPHERE;
	scene->objects[8].sca = init_vec3(1.8f, .25f, 1.9f);
	scene->objects[8].rot = init_vec3(15.f, 0.f, 0.f);
	scene->objects[8].loc = init_vec3(2.45f, 1.0f, -1.9f);
	scene->objects[8].color = init_vec3(0.1f, 0.1f, .1f);
	scene->objects[8].emi = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[8].diffuse = 0.9f;
	scene->objects[8].specular = 0.1f;
	scene->objects[8].spec_exp = 20.0f;

	for (int i = 0; i < scene->num_objects; ++i)
		obj_transform_mats(&scene->objects[i]);
}
