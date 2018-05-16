#include <stdio.h>
#include "rt.h"

void		init_cam(cJSON *j_root, t_scene *scene)
{
	cJSON		*j_camera;

	j_camera = cJSON_GetObjectItem(j_root, "camera");
	if (j_camera)
	{
		scene->cam = (t_camera*)malloc(sizeof(t_camera));
		scene->cam->location = cjcjGetFloat3(j_camera, "location");
		scene->cam->rotation = cjcjGetFloat3(j_camera, "rotation");
		scene->cam->fov = cjGetFloat(j_camera, "fov");
		set_cam_translate_matrix(scene->cam);
		set_cam_rotate_matrix(scene->cam);
	}
	else
	{
		printf("[-] No camera field in scene document\n");
	}
}

void		init_lights(cJSON *j_root, t_scene *scene)
{
	cJSON	*j_lights;
	cJSON	*cur_obj;
	cl_int	i;

	j_lights = cJSON_GetObjectItem(j_root, "lights");
	if (j_lights)
	{
		i = 0;
		scene->num_lights = (cl_int)cJSON_GetArraySize(j_lights);
		scene->lights = (t_light *)malloc(sizeof(t_light) * scene->num_lights);
		while (i < scene->num_lights)
		{
			cur_obj = cJSON_GetArrayItem(j_lights, i);
			scene->lights[i].location = cjcjGetFloat3(cur_obj, "location");
			scene->lights[i].emi = cjcjGetFloat3(cur_obj, "emission");
			i++;
		}
	}
}

void		init_scene(char *strJSON, t_scene *scene)
{
	cJSON *j_root;

	j_root = cJSON_Parse(strJSON);
	if (!j_root)
	{
		printf("[-] JSON Error: %s\n", cJSON_GetErrorPtr());
		exit(0);
	}
	init_cam(j_root, scene);
	init_lights(j_root, scene);
	init_objects(j_root, scene);

	cJSON_Delete(j_root);
}

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
void		print_objects(t_scene *scene)
{
	t_object *obj;

	printf("num lights: %d\n", scene->num_lights);
	printf("num objects: %d\n", scene->num_objects);
	printf("================================\n");
	for (int i = 0; i < scene->num_objects; ++i)
	{
		obj = &scene->objects[i];

		printf("id: %d\n", obj->id);
		printf("type: %d\n", obj->type);
		printf("hidden: %d\n", obj->hidden);
		printf("capped: %d\n", obj->capped);
		printf("operands: %d %d\n", obj->operand.x, obj->operand.y);
		printf("location: %.2f %.2f %.2f\n", obj->location.x, obj->location.y, obj->location.z);
		printf("rotation: %.2f %.2f %.2f\n", obj->rotation.x, obj->rotation.y, obj->rotation.z);
		printf("scale: %.2f %.2f %.2f\n", obj->scale.x, obj->scale.y, obj->scale.z);
		printf("color: %.2f %.2f %.2f\n", obj->color.x, obj->color.y, obj->color.z);
		printf("diffuse: %.2f\n", obj->diffuse);
		printf("specular: %.2f\n", obj->specular);
		printf("specular exp: %.2f\n", obj->spec_exp);
		printf("----------------------------------\n");
	}
}

void		parse_scene(char *scene_path, t_scene *scene)
{
	char *scene_str;

	FILE *fp;

	fp = fopen(scene_path, "r");
	if (!fp)
	{
		fprintf(stderr, "[-] Failed to load scene file.\n");
		exit(0);
	}
	scene_str = (char*)malloc(MAX_SOURCE_SIZE);
	fread(scene_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	init_scene(scene_str, scene);

	for (int i = 0; i < scene->num_objects; ++i)
		obj_transform_mats(&scene->objects[i]);

	print_objects(scene);

	free(scene_str);
	scene_str = NULL;
}