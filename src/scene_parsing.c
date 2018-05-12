#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "rt.h"

cl_float3	cjcjGetFloat3(cJSON *root, char *item_name)
{
	cl_float3	res;
	cJSON		*j_float3_arr;

	res = init_vec3(0.0f, 0.0f, 0.0f);
	j_float3_arr = cJSON_GetObjectItem(root, item_name);
	if (j_float3_arr)
	{
		res.x = (cl_float)cJSON_GetArrayItem(j_float3_arr, 0)->valuedouble;
		res.y = (cl_float)cJSON_GetArrayItem(j_float3_arr, 1)->valuedouble;
		res.z = (cl_float)cJSON_GetArrayItem(j_float3_arr, 2)->valuedouble;
	}
	return res;
}

cl_float	cjGetFloat(cJSON *object, char *item_name)
{
	cJSON		*float_obj;
	cl_float	res;

	res = 0.0f;
	float_obj = cJSON_GetObjectItem(object, item_name);
	if (float_obj)
		res = (cl_float)float_obj->valuedouble;
	return (res);
}

cl_int		cjGetInt(cJSON *object, char *item_name)
{
	cJSON	*int_obj;
	cl_int	res;

	res = 0;
	int_obj = cJSON_GetObjectItem(object, item_name);
	if (int_obj)
		res = (cl_int)int_obj->valueint;
	return (res);
}

char		*cjGetString(cJSON *object, char *item_name)
{
	cJSON	*string_obj;
	char	*res;

	res = NULL;
	string_obj = cJSON_GetObjectItem(object, item_name);
	if (string_obj)
		res = string_obj->valuestring;
	return (res);
}

cl_int		cjGetType(char *string_type)
{
	cl_int	result;

	if (!strcmp(string_type, "PLANE"))
		result = 0;
	else if (!strcmp(string_type, "SPHERE"))
		result = 1;
	else if (!strcmp(string_type, "CYLINDER"))
		result = 2;
	else if (!strcmp(string_type, "CONE"))
		result = 3;
	else if (!strcmp(string_type, "BOX"))
		result = 4;
	else if (!strcmp(string_type, "UNION"))
		result = 5;
	else if (!strcmp(string_type, "INTERSECTION"))
		result = 6;
	else if (!strcmp(string_type, "DIFFERENCE"))
		result = 7;
	else if (!strcmp(string_type, "CLIPPING"))
		result = 8;
	else
		result = 1;

	return result;
}

void		init_cam(cJSON *j_root, t_scene *scene)
{
	cJSON		*j_camera;

	j_camera = cJSON_GetObjectItem(j_root, "camera");

	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	scene->cam->location = cjcjGetFloat3(j_camera, "location");
	scene->cam->rotation = cjcjGetFloat3(j_camera, "rotation");
	scene->cam->fov = cjGetFloat(j_camera, "fov");

	set_cam_translate_matrix(scene->cam);
	set_cam_rotate_matrix(scene->cam);
}

void		init_lights(cJSON *j_root, t_scene *scene)
{
	cJSON	*j_lights;
	cJSON	*cur_obj;
	cl_int	i;

	j_lights = cJSON_GetObjectItem(j_root, "lights");

	scene->num_lights = (cl_int)cJSON_GetArraySize(j_lights);
	scene->lights = (t_light *)malloc(sizeof(t_light) * scene->num_lights);

	printf("num lights: %d\n", scene->num_lights);

	i = 0;
	while (i < scene->num_lights)
	{
		cur_obj = cJSON_GetArrayItem(j_lights, i);
		scene->lights[i].location = cjcjGetFloat3(cur_obj, "location");
		scene->lights[i].emi = cjcjGetFloat3(cur_obj, "emission");
		i++;
	}
}

void		init_empty(t_object *obj)
{
	obj->id = 0;
	obj->type = 0;
	obj->hidden = CL_FALSE;
	obj->capped = CL_FALSE;
	obj->operand = init_int2(0, 0);
	obj->location = init_vec3(0.0f, 0.0f, 0.0f);
	obj->rotation = init_vec3(0.0f, 0.0f, 0.0f);
	obj->scale = init_vec3(1.0f, 1.0f, 1.0f);
	obj->color = init_vec3(0.5f, 0.5f, 0.5f);
	// obj->emi = init_vec3(0.0f, 0.0f, 0.0f);
	obj->diffuse = 1.0f;
	obj->specular = 0.0f;
	obj->spec_exp = 30.0f;
}

void		init_objects(cJSON *j_root, t_scene *scene)
{
	cJSON	*j_objects;
	cJSON	*cur_obj;
	cl_int	i;

	j_objects = cJSON_GetObjectItem(j_root, "objects");

	scene->num_objects = (cl_int)cJSON_GetArraySize(j_objects);
	scene->objects = (t_object *)malloc(sizeof(t_object) * scene->num_objects);

	printf("num objects: %d\n", scene->num_objects);

	i = 0;
	while (i < scene->num_objects)
	{
		init_empty(&scene->objects[i]);

		cur_obj = cJSON_GetArrayItem(j_objects, i);
		scene->objects[i].id = i;
		scene->objects[i].type = cjGetType(cjGetString(cur_obj, "type"));
		scene->objects[i].hidden = (cl_bool)cjGetInt(cur_obj, "hidden");
		scene->objects[i].capped = (cl_bool)cjGetInt(cur_obj, "capped");
		scene->objects[i].location = cjcjGetFloat3(cur_obj, "location");
		scene->objects[i].rotation = cjcjGetFloat3(cur_obj, "rotation");
		scene->objects[i].scale = cjcjGetFloat3(cur_obj, "scale");
		scene->objects[i].color = cjcjGetFloat3(cur_obj, "color");
		// scene->objects[i].emi = cjcjGetFloat3(cur_obj, "emission");
		scene->objects[i].diffuse = cjGetFloat(cur_obj, "diffuse");
		scene->objects[i].specular = cjGetFloat(cur_obj, "specular");
		scene->objects[i].spec_exp = cjGetFloat(cur_obj, "specular_exp");
		i++;
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

	free(scene_str);
	scene_str = NULL;
}