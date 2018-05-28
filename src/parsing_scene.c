#include <stdio.h>
#include "rt.h"

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
void		print_camera(t_scene *scene)
{
    t_camera *camera;

    camera = scene->cam;
    printf("================================\n");
    printf("CAMERA\n");
    printf("================================\n");
    printf("location: %.2f %.2f %.2f\n", camera->location.x, camera->location.y, camera->location.z);
    printf("rotation: %.2f %.2f %.2f\n", camera->rotation.x, camera->rotation.y, camera->rotation.z);
    printf("fov: %.2f\n", scene->cam->fov);
    printf("----------------------------------\n");
}

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
void		print_lights(t_scene *scene)
{
    t_light *light;

    printf("================================\n");
    printf("LIGHT\n");
    printf("================================\n");
    for (int i = 0; i < scene->num_lights; i++)
    {
        light = &scene->lights[i];

        printf("id: %d\n", i);
        printf("type: %d\n", light->type);
        printf("location: %.2f %.2f %.2f\n", light->location.x, light->location.y, light->location.z);
        printf("emission: %.2f %.2f %.2f\n", light->emission.x, light->emission.y, light->emission.z);
        printf("angle: %f\n", light->angle);
        printf("direction: %.2f %.2f %.2f\n", light->dir.x, light->dir.y, light->dir.z);
        printf("----------------------------------\n");
    }
}

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
void		print_objects(t_scene *scene)
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
        else if (obj->type == 9)
            type_name = "BOCAL";
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

void		camera_init_empty(t_scene *scene)
{
    scene->cam->location = init_vec3(0.0f, 1.0f, 10.0f);
    scene->cam->rotation = init_vec3(0.0f, 0.0f, 0.0f);
    scene->cam->fov = 45.0f;
}

void		camera_init(cJSON *cj_root, t_scene *scene)
{
	cJSON		*cj_camera;

	cj_camera = cJSON_GetObjectItem(cj_root, "camera");
	if (cj_camera)
	{
		scene->cam = (t_camera*)malloc(sizeof(t_camera));
        camera_init_empty(scene);
		scene->cam->location = cjGetFloat3(cj_camera, "location");
		scene->cam->rotation = cjGetFloat3(cj_camera, "rotation");
		scene->cam->fov = cjGetFloat(cj_camera, "fov");
		set_cam_translate_matrix(scene->cam);
		set_cam_rotate_matrix(scene->cam);
	}
	else
	{
		printf("[-] No camera field in scene document\n");
        exit(EXIT_FAILURE);
	}
}

void		lights_init(cJSON *cj_root, t_scene *scene)
{
	cJSON	*cj_lights;
	cJSON	*cur_obj;
	cl_int	i;
    cl_int	count_ambient;

	cj_lights = cJSON_GetObjectItem(cj_root, "lights");
	if (cj_lights)
	{
        count_ambient = 0;
		i = 0;
		scene->num_lights = (cl_int)cJSON_GetArraySize(cj_lights);
		scene->lights = (t_light *)malloc(sizeof(t_light) * scene->num_lights);
		while (i < scene->num_lights)
		{
			cur_obj = cJSON_GetArrayItem(cj_lights, i);
            scene->lights[i].type = cjGetLightType(cjGetString(cur_obj, "type"));
			scene->lights[i].location = cjGetFloat3(cur_obj, "location");
            scene->lights[i].emission = cjGetFloat3(cur_obj, "emission");
            scene->lights[i].angle = cjGetFloat(cur_obj, "angle");
            scene->lights[i].dir = cjGetFloat3(cur_obj, "direction");

            if (scene->lights[i].type == AMBIENT)
                count_ambient++;
            if (count_ambient > 1)
            {
                printf("[-] Ambient lights must be only one\n");
                exit(EXIT_FAILURE);
            }

			i++;
		}
	}
    else
    {
        printf("[-] No lights field in scene document\n");
        exit(EXIT_FAILURE);
    }
}

void		scene_init(char *strJSON, t_scene *scene)
{
	cJSON *cj_root;

	cj_root = cJSON_Parse(strJSON);
	if (!cj_root)
	{
		printf("[-] JSON Error: %s\n", cJSON_GetErrorPtr());
		exit(0);
	}
	camera_init(cj_root, scene);
	lights_init(cj_root, scene);

	objects_init(cj_root, scene);

	cJSON_Delete(cj_root);
}

void		parse_scene(char *scene_path, t_scene *scene)
{
	char    *scene_str;
    int     fd;
    int     i;

    if ((fd = open(scene_path, O_RDONLY)) < 0 || read(fd, 0, 0) < 0)
    {
        // Проверить можем ли мы использовать perror
        perror("[-] Read Scene Error");
        close(fd);
        exit(EXIT_FAILURE);
    }

    scene_str = (char *)malloc(MAX_SOURCE_SIZE);
    read(fd, scene_str, MAX_SOURCE_SIZE);
	close(fd);

    if (scene_str[0] != '{') {
        printf("[-] Read Scene Error\n");
        exit(EXIT_FAILURE);
    }

	scene_init(scene_str, scene);

    i = 0;
    while (i < scene->num_objects)
        obj_transform_mats(&scene->objects[i++]);

    /*
     * !! JUST FOR DEBUG !! -------------------------------------------------------
     */
    print_camera(scene);
    print_lights(scene);
    print_objects(scene);

	free(scene_str);
	scene_str = NULL;
}