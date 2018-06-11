#include <stdlib.h>
#include "rt.h"

void actions(t_scene *scene)
{
	if (scene->controls.ismoving != 0)
		move(scene, scene->controls.ismoving);
	if (scene->controls.isstrafing != 0)
		strafe(scene, scene->controls.isstrafing);
	if (scene->controls.isflying != 0)
		fly(scene, scene->controls.isflying);
	if (scene->controls.isrotating != 0)
		rotate(scene, scene->controls.isrotating);
	if (scene->controls.ispitching != 0)
		pitch(scene, scene->controls.ispitching);
}

int		main(void)
{
	t_cl_context	cl_context;
	t_sdl_context	sdl_context;
	t_scene			scene;
	cl_int			ret;

	init_cl(&cl_context);
	init_sdl(&sdl_context);

	// parse_scene("./scenes/test.json", &scene);
	// parse_scene("./scenes/test4.json", &scene);
	open_scene("./scenes/sphere.json", &scene);

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
