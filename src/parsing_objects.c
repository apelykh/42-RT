#include <stdio.h>
#include "rt.h"

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
static void		print_objects(t_scene *scene)
{
    t_object *obj;
    char     *type_name;

    printf("================================\n");
    printf("num lights: %d\n", scene->num_lights);
    printf("num objects: %d\n", scene->num_objects);
    printf("================================\n");
    printf("OBJECTS\n");
    printf("================================\n");
    for (int i = 0; i < scene->num_objects; i++)
    {
        obj = &scene->objects[i];

        printf("id: %d\n", obj->id);
        if (obj->type == 0)
            type_name = "PLANE";
        else if (obj->type == 1)
            type_name = "SPHERE";
        else if (obj->type == 2)
            type_name = "CYLINDER";
        else if (obj->type == 3)
            type_name = "CONE";
        else if (obj->type == 4)
            type_name = "BOX";
        else if (obj->type == 5)
            type_name = "UNION";
        else if (obj->type == 6)
            type_name = "INTERSECTION";
        else if (obj->type == 7)
            type_name = "DIFFERENCE";
        else if (obj->type == 8)
            type_name = "CLIPPING";
        printf("type: %d (%s)\n", obj->type, type_name);
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


static void		object_init_start(t_object *obj)
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

static int		count_inner_objects(cJSON *cj_objects, int count_objects)
{
	cJSON	*cj_inner_objects;
	cJSON	*cj_current_obj;
	int		type;
	int		count_inner;
	int		i;

	i = 0;
    count_inner = 0;
	while (i < count_objects)
	{
        cj_current_obj = cJSON_GetArrayItem(cj_objects, i);
		type = cjGetType(cjGetString(cj_current_obj, "type"));
		if (type >= UNION && type <= CLIPPING)
		{
            cj_inner_objects = cJSON_GetObjectItem(cj_current_obj, "inner_objects");
            count_inner += cJSON_GetArraySize(cj_inner_objects);
		}
		i++;
	}
	return (count_inner);
}

static cJSON	*parse_object(t_object *obj, int obj_id, cJSON *cj_objects, int cj_i)
{
	cJSON	*cj_obj_current;
    cl_int	obj_type;

    cj_obj_current = cJSON_GetArrayItem(cj_objects, cj_i);
    object_init_start(obj);
    obj_type = (cl_int)cjGetType(cjGetString(cj_obj_current, "type"));
	obj->id = (cl_int)obj_id;
	obj->type = obj_type;
    cjGetBool(&(obj->hidden), cj_obj_current, "hidden");
    cjGetBool(&(obj->capped), cj_obj_current, "capped");
	obj->location = clamp_float3_minmax(
		cjGetFloat3(cj_obj_current, "location"), -1000.0f, 1000.0f);
	obj->rotation = clamp_float3_minmax(
		cjGetFloat3(cj_obj_current, "rotation"), -180.0f, 180.0f);
	obj->scale = clamp_float3_minmax(
		cjGetFloat3(cj_obj_current, "scale"), 0.0f, 1000.0f);
	obj->color = clamp_float3_minmax(
		cjGetFloat3(cj_obj_current, "color"), 0.0f, 1.0f);
	obj->diffuse = clamp_float_minmax(
		cjGetFloat(cj_obj_current, "diffuse"), 0.0f, 1.0f);
	obj->specular = clamp_float_minmax(
		cjGetFloat(cj_obj_current, "specular"), 0.0f, 1.0f);
	obj->spec_exp = clamp_float_minmax(
		cjGetFloat(cj_obj_current, "specular_exp"), 0.0f, 300.0f);
	return (cj_obj_current);
}

static void		parse_inner_objects(t_scene *scene, int *obj_parent_id, cJSON *cj_obj_parent)
{
	cJSON	*cj_inner_objects;
	int		cj_inner_i;
    int     obj_inner_id;

    cj_inner_objects = cJSON_GetObjectItem(cj_obj_parent, "inner_objects");

    if (cJSON_GetArraySize(cj_inner_objects) != 2)
    {
        printf("Error: Inner object count must be only 2\n");
        exit(EXIT_FAILURE);
    }

    cj_inner_i = 0;
    obj_inner_id = (*obj_parent_id) + 1;
    while (cj_inner_i < 2)
	{
		parse_object(&scene->objects[obj_inner_id], obj_inner_id, cj_inner_objects, cj_inner_i);

		if (cj_inner_i == 0)
			scene->objects[*obj_parent_id].operand.x = obj_inner_id;
		if (cj_inner_i == 1)
			scene->objects[*obj_parent_id].operand.y = obj_inner_id;
        cj_inner_i++;
        obj_inner_id++;
	}
    (*obj_parent_id) += 2;
}

void		objects_init(cJSON *cj_root, t_scene *scene)
{
	cJSON	*cj_objects;
	cJSON	*cj_obj_current;
	int		count_parent_objects;
	int		obj_type;
	int		obj_id;
    int		cj_i;

	cj_objects = cJSON_GetObjectItem(cj_root, "objects");

    if (cj_objects)
    {
        count_parent_objects = cJSON_GetArraySize(cj_objects);
        scene->num_objects = (cl_int)(6 + count_parent_objects +
                                      count_inner_objects(cj_objects, count_parent_objects));
        scene->objects = (t_object *)malloc(sizeof(t_object) * scene->num_objects);
        bocal_init(scene);

        cj_i = 0;
        obj_id = 6;
        while (cj_i < count_parent_objects)
        {
            cj_obj_current = parse_object(&scene->objects[obj_id], obj_id, cj_objects, cj_i);
            obj_type = scene->objects[obj_id].type;

            if (obj_type >= UNION && obj_type <= CLIPPING)
                parse_inner_objects(scene, &obj_id, cj_obj_current);
            obj_id++;
            cj_i++;
        }
    }
    else
    {
        printf("[-] No objects field in scene document\n");
        exit(EXIT_FAILURE);
    }

    print_objects(scene);
}