#include <stdlib.h>
#include "rt.h"

static void	camera_init(t_scene *scene)
{
	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	scene->cam->location = init_vec3(0.0f, 1.0f, 10.0f);
	scene->cam->rotation = init_vec3(0.f, 0.f, 0.f);
	scene->cam->fov = 45.f;

	set_cam_translate_matrix(scene->cam);
	set_cam_rotate_matrix(scene->cam);
}

static void	lights_init(t_scene *scene)
{	
	scene->num_lights = 3;
	scene->lights = (t_light*)malloc(sizeof(t_light) * scene->num_lights);
	scene->lights[0].location = init_vec3(0.0f, 2.75f, 5.75f);
	scene->lights[0].emi = init_vec3(0.8f, 0.8f, 0.8f);
	scene->lights[1].location = init_vec3(3.0f, 3.0f, -40.0f);
	scene->lights[1].emi = init_vec3(0.5f, 0.5f, 0.5f);
	scene->lights[2].location = init_vec3(30.0f, 5.0f, 2.0f);
	scene->lights[2].emi = init_vec3(0.6f, 0.6f, 0.6f);
}

void	init_scene1(t_scene *scene)
{
	camera_init(scene);
	lights_init(scene);

	scene->num_objects = 8;
	scene->objects = (t_object*)malloc(sizeof(t_object) * scene->num_objects);
	
	scene->objects[0].id = 0;
	scene->objects[0].type = DIFFERENCE;
	scene->objects[0].operand.x = 1;
	scene->objects[0].operand.y = 2;
	scene->objects[0].capped = CL_FALSE;
	scene->objects[0].hidden = CL_FALSE;
	scene->objects[0].scale = init_vec3(1.f, 3.f, 1.f);
	scene->objects[0].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[0].location = init_vec3(0.f, 0.0f, 0.0f);
	scene->objects[0].color = init_vec3(0.0f, 0.9f, 0.0f);
	scene->objects[0].diffuse = 0.7f;
	scene->objects[0].specular = 0.9f;
	scene->objects[0].spec_exp = 20.0f;

	scene->objects[1].id = 1;
	scene->objects[1].type = BOX;
	scene->objects[1].capped = CL_TRUE;
	scene->objects[1].hidden = CL_TRUE;
	scene->objects[1].scale = init_vec3(1.f, 1.f, 1.f);
	scene->objects[1].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[1].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].color = init_vec3(0.9f, 0.0f, 0.0f);
	scene->objects[1].diffuse = 0.7f;
	scene->objects[1].specular = 0.9f;
	scene->objects[1].spec_exp = 20.0f;

	scene->objects[2].id = 2;
	scene->objects[2].type = CONE;
	scene->objects[2].capped = CL_FALSE;
	scene->objects[2].hidden = CL_TRUE;
	scene->objects[2].scale = init_vec3(1.f, 1.0f, 1.f);
	scene->objects[2].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[2].location = init_vec3(0.5f, 0.f, 0.5f);
	scene->objects[2].color = init_vec3(0.0f, 0.0f, 0.9f);
	scene->objects[2].diffuse = 0.7f;
	scene->objects[2].specular = 0.9f;
	scene->objects[2].spec_exp = 20.0f;

	// scene->objects[0].id = 0;
	// scene->objects[0].type = INTERSECTION;
	// scene->objects[0].operand.x = 1;
	// scene->objects[0].operand.y = 2;
	// scene->objects[0].capped = CL_FALSE;
	// scene->objects[0].hidden = CL_FALSE;
	// scene->objects[0].scale = init_vec3(.5f, .25f, 1.f);
	// scene->objects[0].rotation = init_vec3(0.f, -45.f, 0.f);
	// scene->objects[0].location = init_vec3(0.f, 0.0f, 0.0f);
	// scene->objects[0].color = init_vec3(0.58f, 0.29f, 0.0f);
	// scene->objects[0].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[0].diffuse = 0.7f;
	// scene->objects[0].specular = 0.9f;
	// scene->objects[0].spec_exp = 20.0f;

	scene->objects[3].id = 3;
	scene->objects[3].type = BOX;
	scene->objects[3].capped = CL_TRUE;
	scene->objects[3].hidden = CL_TRUE;
	scene->objects[3].scale = init_vec3(1.f, 1.f, 1.f);
	scene->objects[3].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[3].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[3].color = init_vec3(0.0f, 0.0f, 0.9f);
	scene->objects[3].diffuse = 0.7f;
	scene->objects[3].specular = 0.9f;
	scene->objects[3].spec_exp = 20.0f;

	scene->objects[4].id = 4;
	scene->objects[4].type = SPHERE;
	scene->objects[4].capped = CL_FALSE;
	scene->objects[4].hidden = CL_TRUE;
	scene->objects[4].scale = init_vec3(1.1f, 1.1f, .9f);
	scene->objects[4].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[4].location = init_vec3(0.15f, 0.5f, 0.f);
	scene->objects[4].color = init_vec3(0.9f, 0.0f, 0.0f);
	scene->objects[4].diffuse = 0.7f;
	scene->objects[4].specular = 0.9f;
	scene->objects[4].spec_exp = 20.0f;

	// scene->objects[5].id = 5;
	// scene->objects[5].type = CONE;
	// scene->objects[5].capped = CL_TRUE;
	// scene->objects[5].hidden = CL_FALSE;
	// scene->objects[5].scale = init_vec3(.75f, 2.f, .75f);
	// scene->objects[5].rotation = init_vec3(0.f, 0.f, 180.f);
	// scene->objects[5].location = init_vec3(4.25f, 0.25f, 0.0f);
	// scene->objects[5].color = init_vec3(0.9f, 0.0f, 0.0f);
	// scene->objects[5].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[5].diffuse = 0.7f;
	// scene->objects[5].specular = 0.9f;
	// scene->objects[5].spec_exp = 20.0f;

	scene->objects[5].id = 5;
	scene->objects[5].type = PLANE;
	scene->objects[5].hidden = CL_FALSE;
	scene->objects[5].scale = init_vec3(1.0f, 1.0f, 1.0f);
	scene->objects[5].rotation = init_vec3(0.f, 0.f, 0.f);
	scene->objects[5].location = init_vec3(0.0f, -1.0f, 0.0f);
	scene->objects[5].color = init_vec3(0.9f, 0.8f, 0.7f);
	scene->objects[5].diffuse = 1.0f;
	scene->objects[5].specular = 0.0f;
	scene->objects[5].spec_exp = 0.5f;

	scene->objects[6].id = 6;
	scene->objects[6].type = CONE;
	scene->objects[6].hidden = CL_FALSE;
	scene->objects[6].scale = init_vec3(.5f, 1.f, .5f);
	scene->objects[6].rotation = init_vec3(-15.f, 0.f, -15.f);
	scene->objects[6].location = init_vec3(0.0f, 2.0f, -2.0f);
	scene->objects[6].color = init_vec3(0.95f, 0.0f, 0.0f);
	scene->objects[6].capped = CL_FALSE;
	scene->objects[6].diffuse = 0.8f;
	scene->objects[6].specular = 0.8f;
	scene->objects[6].spec_exp = 20.0f;

	// scene->objects[8].id = 8;
	// scene->objects[8].type = SPHERE;
	// scene->objects[8].hidden = CL_FALSE;
	// scene->objects[8].scale = init_vec3(5.f, .5f, 1.f);
	// scene->objects[8].rotation = init_vec3(45.f, 0.f, 0.f);
	// scene->objects[8].location = init_vec3(2.5f, 1.0f, -2.0f);
	// scene->objects[8].color = init_vec3(0.98f, 0.68f, .82f);
	// scene->objects[8].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[8].diffuse = 0.7f;
	// scene->objects[8].specular = 0.9f;
	// scene->objects[8].spec_exp = 20.0f;

	// scene->objects[9].id = 9;
	// scene->objects[9].type = SPHERE;
	// scene->objects[9].hidden = CL_FALSE;
	// scene->objects[9].scale = init_vec3(2.f, 2.f, 2.f);
	// scene->objects[9].rotation = init_vec3(0.f, 0.f, 0.f);
	// scene->objects[9].location = init_vec3(2.5f, 1.0f, -2.0f);
	// scene->objects[9].color = init_vec3(0.98f, 0.68f, .82f);
	// scene->objects[9].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[9].diffuse = 0.9f;
	// scene->objects[9].specular = 0.9f;
	// scene->objects[9].spec_exp = 20.0f;

	scene->objects[7].id = 7;
	scene->objects[7].type = BOX;
	scene->objects[7].hidden = CL_FALSE;
	scene->objects[7].scale = init_vec3(2.f, 3.f, .05f);
	scene->objects[7].rotation = init_vec3(0.f, 60.f, -30.f);
	scene->objects[7].location = init_vec3(-2.5f, 1.0f, -2.0f);
	scene->objects[7].color = init_vec3(1.0f, 0.49f, 0.0f);
	scene->objects[7].diffuse = 0.9f;
	scene->objects[7].specular = 0.9f;
	scene->objects[7].spec_exp = 20.0f;

	// scene->objects[11].id = 11;
	// scene->objects[11].type = CYLINDER;
	// scene->objects[11].hidden = CL_FALSE;
	// scene->objects[11].scale = init_vec3(.1f, 8.f, .1f);
	// scene->objects[11].rotation = init_vec3(0.f, 0.f, 90.f);
	// scene->objects[11].location = init_vec3(0.0f, 2.0f, -2.1f);
	// scene->objects[11].color = init_vec3(.3f, 0.3f, 0.3f);
	// scene->objects[11].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[11].capped = CL_FALSE;
	// scene->objects[11].diffuse = 0.7f;
	// scene->objects[11].specular = 0.9f;
	// scene->objects[11].spec_exp = 20.0f;

	// scene->objects[12].id = 12;
	// scene->objects[12].type = SPHERE;
	// scene->objects[12].hidden = CL_FALSE;
	// scene->objects[12].scale = init_vec3(.3f, .25f, .3f);
	// scene->objects[12].rotation = init_vec3(0.f, 0.f, 0.f);
	// scene->objects[12].location = init_vec3(2.f, 1.5f, -1.1f);
	// scene->objects[12].color = init_vec3(0.1f, 0.1f, .1f);
	// scene->objects[12].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[12].diffuse = 0.9f;
	// scene->objects[12].specular = 0.9f;
	// scene->objects[12].spec_exp = 20.0f;

	// scene->objects[13].id = 13;
	// scene->objects[13].type = SPHERE;
	// scene->objects[13].hidden = CL_FALSE;
	// scene->objects[13].scale = init_vec3(.3f, .25f, .3f);
	// scene->objects[13].rotation = init_vec3(0.f, 0.f, 0.f);
	// scene->objects[13].location = init_vec3(2.6f, 1.5f, -1.1f);
	// scene->objects[13].color = init_vec3(0.1f, 0.1f, .1f);
	// scene->objects[13].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[13].diffuse = 0.9f;
	// scene->objects[13].specular = 0.9f;
	// scene->objects[13].spec_exp = 20.0f;

	// scene->objects[14].id = 14;
	// scene->objects[14].type = SPHERE;
	// scene->objects[14].hidden = CL_FALSE;
	// scene->objects[14].scale = init_vec3(1.8f, .25f, 1.9f);
	// scene->objects[14].rotation = init_vec3(15.f, 0.f, 0.f);
	// scene->objects[14].location = init_vec3(2.45f, 1.0f, -1.9f);
	// scene->objects[14].color = init_vec3(0.1f, 0.1f, .1f);
	// scene->objects[14].emi = init_vec3(0.0f, 0.0f, 0.0f);
	// scene->objects[14].diffuse = 0.9f;
	// scene->objects[14].specular = 0.1f;
	// scene->objects[14].spec_exp = 20.0f;

	for (int i = 0; i < scene->num_objects; ++i)
		obj_transform_mats(&scene->objects[i]);
}
