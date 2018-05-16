#include <stdio.h>
#include "rt.h"

void		init_empty_obj(t_object *obj)
{
	obj->id = 0;
	obj->type = 0;
	obj->hidden = CL_FALSE;
	obj->capped = CL_FALSE;
	obj->operand = init_int2(-1, -1);
	obj->location = init_vec3(0.0f, 0.0f, 0.0f);
	obj->rotation = init_vec3(0.0f, 0.0f, 0.0f);
	obj->scale = init_vec3(1.0f, 1.0f, 1.0f);
	obj->color = init_vec3(0.5f, 0.5f, 0.5f);
	obj->diffuse = 1.0f;
	obj->specular = 0.0f;
	obj->spec_exp = 1.0f;
}

static int		count_inner_objects(cJSON *j_objects, int num_objects)
{
	cJSON	*inner_objects;
	cJSON	*cur_obj;
	int		type;
	int		num_inner;
	int		i;

	i = 0;
	num_inner = 0;
	while (i < num_objects)
	{
		cur_obj = cJSON_GetArrayItem(j_objects, i);
		type = cjGetType(cjGetString(cur_obj, "type"));
		if (type >= 5 && type <= 8)
		{
			inner_objects = cJSON_GetObjectItem(cur_obj, "inner_objects");
			num_inner += cJSON_GetArraySize(inner_objects);
		}
		i++;
	}
	return (num_inner);
}

static cJSON	*parse_object(t_object *obj, int id, cJSON *j_objects, int i)
{
	cJSON	*cur_obj;
	int		type;

	init_empty_obj(obj);
	cur_obj = cJSON_GetArrayItem(j_objects, i);
	type = cjGetType(cjGetString(cur_obj, "type"));
	obj->id = (cl_int)id;
	obj->type = (cl_int)type;
	obj->hidden = (cl_bool)cjGetInt(cur_obj, "hidden");
	obj->capped = (cl_bool)cjGetInt(cur_obj, "capped");
	obj->location = clamp_float3_minmax(
		cjcjGetFloat3(cur_obj, "location"), -1000.0f, 1000.0f);
	obj->rotation = clamp_float3_minmax(
		cjcjGetFloat3(cur_obj, "rotation"), -180.0f, 180.0f);
	obj->scale = clamp_float3_minmax(
		cjcjGetFloat3(cur_obj, "scale"), 0.0f, 1000.0f);
	obj->color = clamp_float3_minmax(
		cjcjGetFloat3(cur_obj, "color"), 0.0f, 1.0f);
	obj->diffuse = clamp_float_minmax(
		cjGetFloat(cur_obj, "diffuse"), 0.0f, 1.0f);
	obj->specular = clamp_float_minmax(
		cjGetFloat(cur_obj, "specular"), 0.0f, 1.0f);
	obj->spec_exp = clamp_float_minmax(
		cjGetFloat(cur_obj, "specular_exp"), 0.0f, 300.0f);
	return (cur_obj);
}

static void		parse_inner_objects(t_scene *scene, cJSON *cur_obj, int i, int *id)
{
	cJSON	*inner_objects;
	int		j;

	j = 0;
	inner_objects = cJSON_GetObjectItem(cur_obj, "inner_objects");
	while (j < cJSON_GetArraySize(inner_objects))
	{
		parse_object(&scene->objects[*id], *id, inner_objects, j);

		if (j == 0)
			scene->objects[i].operand.x = i + 1;
		if (j == 1)
			scene->objects[i].operand.y = i + 2;
		j++;
		(*id)++;
	}
}

void		init_objects(cJSON *j_root, t_scene *scene)
{
	cJSON	*j_objects;
	cJSON	*cur_obj;
	int		num_objects;
	int		type;
	int		i;
	int		id;

	j_objects = cJSON_GetObjectItem(j_root, "objects");
	num_objects = cJSON_GetArraySize(j_objects);
	scene->num_objects = (cl_int)(num_objects +
		count_inner_objects(j_objects, num_objects));
	scene->objects = (t_object *)malloc(sizeof(t_object) * scene->num_objects);

	i = 0;
	id = 0;
	while (i < num_objects)
	{
		cur_obj = parse_object(&scene->objects[id], id, j_objects, i);
		type = scene->objects[id].type;
		id++;

		if (type >= UNION && type <= CLIPPING)
			parse_inner_objects(scene, cur_obj, i, &id);
		i++;
	}
}