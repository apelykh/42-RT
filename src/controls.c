#include "rt.h"

void	move_z(t_scene *scene, int dir)
{
	cl_float4 vec;

	vec = mat_mult_vec(scene->cam->rotate_matrix, init_vec4(0.f, 0.f, dir * -0.5f, 0.f));
	scene->cam->location = add_vec3(scene->cam->location, vec);
	set_cam_translate_matrix(scene->cam);
}

void	move_x(t_scene *scene, int dir)
{
	cl_float4 vec;
	
	vec = mat_mult_vec(scene->cam->rotate_matrix, init_vec4(dir * 0.5f, 0.f, 0.0f, 0.f));
	scene->cam->location = add_vec3(scene->cam->location, vec);
	set_cam_translate_matrix(scene->cam);
}

void	move_y(t_scene *scene, int dir)
{
	cl_float4 vec;

	vec = mat_mult_vec(scene->cam->rotate_matrix, init_vec4(0.f, dir * 0.5f, 0.f, 0.f));
	scene->cam->location = add_vec3(scene->cam->location, vec);
	set_cam_translate_matrix(scene->cam);
}

void	rotate_y(t_scene *scene, int dir)
{
	scene->cam->rotation.y += dir * 2.5f;
	set_cam_rotate_matrix(scene->cam);
}

void	rotate_x(t_scene *scene, int dir)
{
	scene->cam->rotation.x += dir * 2.5f;
	set_cam_rotate_matrix(scene->cam);	
}

void	toggle_sepia(t_scene *scene)
{
	if (scene->sepia == 1)
		scene->sepia = 0;
	else
		scene->sepia = 1;
}
