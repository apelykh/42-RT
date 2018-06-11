#include "rt.h"

void print_camera(t_scene *scene)
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

void print_lights(t_scene *scene)
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
        printf("kr: %.2f\n", obj->kr);
        printf("ior exp: %.2f\n", obj->ior);
        printf("----------------------------------\n");
    }
}
