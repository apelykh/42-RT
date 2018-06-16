/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:24:35 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 21:46:58 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "rt.h"

static void	global_init(char *scene_path, t_scene *scene,
	t_sdl_context *sdl_context, t_cl_context *cl_context)
{
	open_scene(scene_path, scene);
	cl_init(cl_context);
	sdl_init(sdl_context);
	cl_alloc_buffers(cl_context, scene);
	cl_set_kernel_args(cl_context, scene);
}

static void	handle_keys(t_scene *scene, SDL_Event *event)
{
	while (SDL_PollEvent(event))
	{
		if (event->type == SDL_QUIT)
			scene->controls.quit = 1;
		else if (event->type == SDL_KEYDOWN)
			key_press(scene, *event);
		else if (event->type == SDL_KEYUP)
			key_release(scene, *event);
	}
	if (scene->controls.move_z != 0)
		move_z(scene, scene->controls.move_z);
	if (scene->controls.move_x != 0)
		move_x(scene, scene->controls.move_x);
	if (scene->controls.move_y != 0)
		move_y(scene, scene->controls.move_y);
	if (scene->controls.rotate_y != 0)
		rotate_y(scene, scene->controls.rotate_y);
	if (scene->controls.rotate_x != 0)
		rotate_x(scene, scene->controls.rotate_x);
}

static void	global_cleanup(t_scene *scene, t_sdl_context *sdl_context,
	t_cl_context *cl_context)
{
	sdl_cleanup(sdl_context);
	cl_cleanup(cl_context);
	free(scene->objects);
	free(scene->lights);
	free(scene->cam);
}

int			main(int argc, char **argv)
{
	t_cl_context	cl_context;
	t_sdl_context	sdl_context;
	t_scene			scene;
	SDL_Event		event;

	if (argc != 2)
		ft_error("[-] Arguments: Invalid number of arguments",
			"Usage: ./RT [scene_path]");
	global_init(argv[1], &scene, &sdl_context, &cl_context);
	while (!scene.controls.quit)
	{
		scene.to_save_image = 0;
		handle_keys(&scene, &event);
		cl_update_pixels(&scene, &sdl_context, &cl_context);
		if (scene.to_save_image)
			save_image(sdl_context.pixels);
		sdl_render(&sdl_context, &scene);
	}
	global_cleanup(&scene, &sdl_context, &cl_context);
	return (0);
}
