#include <stdio.h>
#include "rt.h"

static void controls_init(t_scene *scene)
{
    scene->controls.quit = 0;
    scene->controls.ismoving = 0;
    scene->controls.isstrafing = 0;
    scene->controls.isflying = 0;
    scene->controls.isrotating = 0;
    scene->controls.ispitching = 0;
}

void        scene_init(char *strJSON, t_scene *scene)
{
    cJSON *cj_root;

    cj_root = cJSON_Parse(strJSON);
    if (!cj_root)
        parsing_error("[-] JSON Error", (char *)cJSON_GetErrorPtr());

    scene->im_width = WIN_WIDTH;
    scene->im_height = WIN_HEIGHT;
    controls_init(scene);

    camera_init(cj_root, scene);
    lights_init(cj_root, scene);
    objects_init(cj_root, scene);

    cJSON_Delete(cj_root);
}

void        parse_scene(char *scene_path, t_scene *scene)
{
    char    *scene_str;
    int     fd;
    int     i;

    if ((fd = open(scene_path, O_RDONLY)) < 0 || read(fd, 0, 0) < 0)
    {
        close(fd);
        // Проверить можем ли мы использовать perror
        perror("[-] Read Scene Error");
        exit(EXIT_FAILURE);
    }

    scene_str = (char *)malloc(MAX_SOURCE_SIZE);
    read(fd, scene_str, MAX_SOURCE_SIZE);
    close(fd);

    if (scene_str[0] != '{')
        parsing_error("[-] Read Scene Error", "No first bracket");
    scene_init(scene_str, scene);

    i = 0;
    while (i < scene->num_objects)
        obj_transform_mats(&scene->objects[i++]);
    free(scene_str);
    scene_str = NULL;
}
