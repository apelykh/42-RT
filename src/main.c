#include <stdlib.h>
#include "rt.h"

void	actions(t_scene *scene)
{
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

int		main(int argc, char **argv)
{
	t_cl_context	cl_context;
	t_sdl_context	sdl_context;
	t_scene			scene;
	cl_int			ret;

	if (argc != 2)
		ft_error("[-] Arguments: Invalid number of arguments",
			"Usage: ./RT scene.json");

	open_scene(argv[1], &scene);
	init_cl(&cl_context);
	init_sdl(&sdl_context);

	alloc_cl_buffers(&cl_context, &scene);
	set_kernel_args(&cl_context, &scene);
 
	size_t global_item_size = scene.im_width * scene.im_height;

	SDL_Event	event;

	while (!scene.controls.quit)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				scene.controls.quit = 1;
			else if (event.type == SDL_KEYDOWN)
				key_press(&scene, event);
			else if (event.type == SDL_KEYUP)
				key_release(&scene, event);
		}
		actions(&scene);

		ret = clEnqueueWriteBuffer(cl_context.command_queue, cl_context.cam_buf,
			CL_TRUE, 0, sizeof(t_camera), scene.cam, 0, NULL, NULL);

		ret = clSetKernelArg(cl_context.kernel, 7, sizeof(cl_mem),
			(void*)&cl_context.cam_buf);

		ret = clSetKernelArg(cl_context.kernel, 8, sizeof(cl_mem),
			(void*)&scene.sepia);

		ret = clEnqueueNDRangeKernel(cl_context.command_queue, cl_context.kernel, 1, NULL, 
			&global_item_size, NULL, 0, NULL, NULL);

		ret = clEnqueueReadBuffer(cl_context.command_queue, cl_context.pixels_buf, CL_TRUE, 0,
			scene.im_width * scene.im_height * sizeof(cl_uchar4), sdl_context.pixels, 0, NULL, NULL);

		if (ret != CL_SUCCESS)
			printf("[-] Updating by event went wrong!\n");

		sdl_context.surf = SDL_CreateRGBSurfaceFrom(sdl_context.pixels,
			scene.im_width, scene.im_height, 32, scene.im_width * 4, 0, 0, 0, 0);
		sdl_context.tex = SDL_CreateTextureFromSurface(sdl_context.rend, sdl_context.surf);

		SDL_RenderClear(sdl_context.rend);
		SDL_RenderCopy(sdl_context.rend, sdl_context.tex, NULL, NULL);
		SDL_RenderPresent(sdl_context.rend);

		SDL_FreeSurface(sdl_context.surf);
		SDL_DestroyTexture(sdl_context.tex);
	}
	sdl_cleanup(&sdl_context);
	cl_cleanup(&cl_context);

	free(scene.objects);
	free(scene.lights);
	free(scene.cam);
 
	return (0);
}	
