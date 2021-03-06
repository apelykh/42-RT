/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_objects.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 20:32:22 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/17 19:15:31 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void		object_init(t_object *obj, int obj_id, cJSON *cj_current)
{
	cl_int	obj_type;

	obj_type = (cl_int)cj_get_obj_type(cj_get_str(cj_current, "type"));
	obj->id = (cl_int)obj_id;
	obj->type = obj_type;
	cj_get_bool(&(obj->hidden), cj_current, "hidden");
	cj_get_bool(&(obj->capped), cj_current, "capped");
	save_float3(&(obj->location),
				cj_obj(cj_current, "location"), -100.0f, 100.0f);
	save_float3(&(obj->rotation),
				cj_obj(cj_current, "rotation"), -180.0f, 180.0f);
	save_float3(&(obj->scale), cj_obj(cj_current, "scale"), 0.1f, 100.0f);
	save_float3(&(obj->color), cj_obj(cj_current, "color"), 0.0f, 1.0f);
	save_float(&(obj->transparency), cj_obj(cj_current, "transparency"),
		0.0f, 1.0f);
	save_float(&(obj->spec_pow),
				cj_obj(cj_current, "spec_pow"), 0.0f, 300.0f);
	save_float(&(obj->kr), cj_obj(cj_current, "kr"), 0.0f, 1.0f);
	save_float(&(obj->ior), cj_obj(cj_current, "ior"), 0.0f, 3.0f);
}

static cJSON	*parse_object(t_object *obj, int obj_id, cJSON *cj_objects,
								int cj_i)
{
	cJSON	*cj_current;

	if (!(cj_current = cJSON_GetArrayItem(cj_objects, cj_i)))
		ft_error("[-] Parsing: Object does not exist", NULL);
	object_init_empty(obj);
	object_init(obj, obj_id, cj_current);
	return (cj_current);
}

static void		parse_inner_objects(t_scene *scene, int *obj_parent_id,
									cJSON *cj_obj_parent)
{
	cJSON	*cj_inner_objects;
	int		cj_inner_i;
	int		obj_inner_id;

	cj_inner_objects = cJSON_GetObjectItem(cj_obj_parent, "inner_objects");
	if (cJSON_GetArraySize(cj_inner_objects) != 2)
		ft_error("[-] Parsing: For complex objects "
				"inner object count must be only 2", NULL);
	cj_inner_i = 0;
	obj_inner_id = (*obj_parent_id) + 1;
	while (cj_inner_i < 2)
	{
		parse_object(&scene->objects[obj_inner_id], obj_inner_id,
					cj_inner_objects, cj_inner_i);
		if (cj_inner_i == 0)
			scene->objects[*obj_parent_id].operand.x = obj_inner_id;
		if (cj_inner_i == 1)
			scene->objects[*obj_parent_id].operand.y = obj_inner_id;
		cj_inner_i++;
		obj_inner_id++;
	}
	(*obj_parent_id) += 2;
}

static void		objects_loop(t_scene *scene, cJSON *cj_objects,
							int parent_objects_num)
{
	cJSON	*cj_obj_current;
	int		obj_id;
	int		cj_i;

	cj_i = 0;
	obj_id = 6;
	while (cj_i < parent_objects_num)
	{
		cj_obj_current = parse_object(&scene->objects[obj_id], obj_id,
				cj_objects, cj_i);
		if (scene->objects[obj_id].type >= UNION &&
			scene->objects[obj_id].type <= CLIPPING)
			parse_inner_objects(scene, &obj_id, cj_obj_current);
		obj_id++;
		cj_i++;
	}
}

void			parse_objects(cJSON *cj_root, t_scene *scene)
{
	cJSON	*cj_objects;
	int		count_parent_objects;

	if (!(cj_objects = cJSON_GetObjectItem(cj_root, "objects")))
		ft_error("[-] Parsing: No objects field in scene document", NULL);
	count_parent_objects = cJSON_GetArraySize(cj_objects);
	if (count_parent_objects == 0)
		ft_error("[-] Parsing: Objects array is empty.", NULL);
	scene->num_objects = (cl_int)(6 + count_parent_objects
			+ count_inner_objects(cj_objects, count_parent_objects));
	scene->objects = (t_object *)malloc(sizeof(t_object) * scene->num_objects);
	bocal_init(scene);
	objects_loop(scene, cj_objects, count_parent_objects);
}
