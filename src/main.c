/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:24:35 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/15 22:51:28 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "rt.h"

static void global_init(char *scene_path, t_scene *scene,
	t_sdl_context *sdl_context, t_cl_context *cl_context)
{
	open_scene(scene_path, scene);
	init_cl(cl_context);
	sdl_init(sdl_context);
	alloc_cl_buffers(cl_context, scene);
	set_kernel_args(cl_context, scene);
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


static void	main_loop(t_scene *scene, t_sdl_context	*sdl_context,
	t_cl_context *cl_context)
{
	size_t		global_item_size;
	SDL_Event	event;
	cl_int		ret;

	while (!scene->controls.quit)
	{
		handle_keys(scene, &event);

		ret = clEnqueueWriteBuffer(cl_context->command_queue,
			cl_context->cam_buf, CL_TRUE, 0, sizeof(t_camera),
			scene->cam, 0, NULL, NULL);

		ret = clSetKernelArg(cl_context->kernel, 7, sizeof(cl_mem),
			(void*)&cl_context->cam_buf);

		ret = clSetKernelArg(cl_context->kernel, 8, sizeof(cl_mem),
			(void*)&scene->sepia);

		global_item_size = scene->im_width * scene->im_height;
		ret = clEnqueueNDRangeKernel(cl_context->command_queue,
			cl_context->kernel, 1, NULL, &global_item_size,
			NULL, 0, NULL, NULL);

		ret = clEnqueueReadBuffer(cl_context->command_queue,
			cl_context->pixels_buf, CL_TRUE, 0,
			scene->im_width * scene->im_height * sizeof(cl_uchar4),
			sdl_context->pixels, 0, NULL, NULL);

		if (ret != CL_SUCCESS)
			ft_error("[-] Updating by event went wrong!", "");

		sdl_render(sdl_context, scene);
	}
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

	if (argc != 2)
		ft_error("[-] Arguments: Invalid number of arguments",
			"Usage: ./RT scene.json");

	global_init(argv[1], &scene, &sdl_context, &cl_context);
	main_loop(&scene, &sdl_context, &cl_context);
	global_cleanup(&scene, &sdl_context, &cl_context);
	return (0);
}	
