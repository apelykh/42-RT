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
    camera->location = init_vec3(0.0f, 1.0f, 10.0f);
    camera->rotation = init_vec3(0.0f, 0.0f, 0.0f);
    camera->fov = 45.0f;
}

void		    camera_init(cJSON *cj_root, t_scene *scene)
{
    cJSON		*cj_camera;

    cj_camera = cJSON_GetObjectItem(cj_root, "camera");
    if (cj_camera)
    {
        /* Add limits */
        scene->cam = (t_camera*)malloc(sizeof(t_camera));
        camera_init_start(scene->cam);
        scene->cam->location = cjGetFloat3(cj_camera, "location");
        scene->cam->rotation = cjGetFloat3(cj_camera, "rotation");
        scene->cam->fov = cjGetFloat(cj_camera, "fov");
        set_cam_translate_matrix(scene->cam);
        set_cam_rotate_matrix(scene->cam);
    }
    else
        parsing_error("[-] No camera field in scene document", NULL);
    print_camera(scene);
}
