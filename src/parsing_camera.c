#include "rt.h"

/*
 * !! JUST FOR DEBUG !! -------------------------------------------------------
 */
static void		print_camera(t_scene *scene)
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

static void		camera_init_start(t_camera *camera)
{
    /* Take start values from Anton & Nikita */
    camera->location = init_vec3(-101.0f, 2.0f, 13.0f);
    camera->rotation = init_vec3(0.0f, 0.0f, 0.0f);
    camera->fov = 45.0f;
}

void		    camera_init(cJSON *cj_root, t_scene *scene)
{
    cJSON		*cj_camera;

    cj_camera = cJSON_GetObjectItem(cj_root, "camera");
    if (cj_camera)
    {
        /* Take limits from Anton & Nikita */
        scene->cam = (t_camera*)malloc(sizeof(t_camera));
        camera_init_start(scene->cam);
        save_float3(&(scene->cam->location), cjObj(cj_camera, "location"), -100.0f, 100.0f);
        save_float3(&(scene->cam->rotation), cjObj(cj_camera, "rotation"), -180.0f, 180.0f);
        save_float(&(scene->cam->fov), cjObj(cj_camera, "fov"), 30.0f, 110.0f);
        set_cam_translate_matrix(scene->cam);
        set_cam_rotate_matrix(scene->cam);
    }
    else
        parsing_error("[-] No camera field in scene document", NULL);
    print_camera(scene);
}
