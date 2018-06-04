#include <stdio.h>
#include "rt.h"

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
static void print_lights(t_scene *scene)
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

static void light_init_start(t_light *light)
{
    /* Take start values from Anton & Nikita */
    light->type = 0;
    light->location = init_vec3(0.0f, 0.0f, 0.0f);
    light->emission = init_vec3(0.0f, 0.0f, 0.0f);
    light->dir = init_vec3(1.0f, 1.0f, 1.0f);
    light->angle = 0.0f;
}

void        lights_init(cJSON *cj_root, t_scene *scene)
{
    cJSON	*cj_lights;
    cJSON	*cj_current;
    cl_int	i;
    cl_int	count_ambient;

    if (!(cj_lights = cJSON_GetObjectItem(cj_root, "lights")))
        parsing_error("[-] Parsing Error: Object does not exist", NULL);
    if (cj_lights)
    {
        count_ambient = 0;
        i = 0;
        scene->num_lights = (cl_int)cJSON_GetArraySize(cj_lights);
        scene->lights = (t_light *)malloc(sizeof(t_light) * scene->num_lights);
        while (i < scene->num_lights)
        {
            /* Take limits from Anton & Nikita */
            cj_current = cJSON_GetArrayItem(cj_lights, i);
            light_init_start(&(scene->lights[i]));
            scene->lights[i].type = cjGetLightType(cjGetString(cj_current, "type"));
            save_float3(&(scene->lights[i].location), cjObj(cj_current, "location"), -100.0f, 100.0f);
            save_float3(&(scene->lights[i].emission), cjObj(cj_current, "emission"), -180.0f, 180.0f);
            save_float(&(scene->lights[i].angle), cjObj(cj_current, "angle"), 0.0f, 90.0f);
            save_float3(&(scene->lights[i].dir), cjObj(cj_current, "direction"), -180.0f, 180.0f);

            if (scene->lights[i].type == AMBIENT)
                count_ambient++;
            if (count_ambient > 1)
                parsing_error("[-] Ambient lights must be only one", NULL);
            i++;
        }
    }
    else
        parsing_error("[-] No lights field in scene document", NULL);
    print_lights(scene);
}
