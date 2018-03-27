#include <stdlib.h>
#include "rt.h"

int		main(void)
{
	t_cl_context cl_context;
	t_sdl_context sdl_context;

	t_scene scene;

	cl_int ret;

	scene.im_width = WIN_WIDTH;
	scene.im_height = WIN_HEIGHT;
 
	cl_float3 *pixels;
	pixels = (cl_float3*)malloc(scene.im_width * scene.im_height * sizeof(cl_float3));
 
	init_cl(&cl_context);
	init_sdl(&sdl_context);

	init_scene1(&scene);
	// printf("%f %f %f %f\n", scene.objects[1].w2l.s0, scene.objects[1].w2l.s1, scene.objects[1].w2l.s2, scene.objects[1].w2l.s3);
	// printf("%f %f %f %f\n", scene.objects[1].w2l.s4, scene.objects[1].w2l.s5, scene.objects[1].w2l.s6, scene.objects[1].w2l.s7);
	// printf("%f %f %f %f\n", scene.objects[1].w2l.s8, scene.objects[1].w2l.s9, scene.objects[1].w2l.sA, scene.objects[1].w2l.sB);
	// printf("%f %f %f %f\n", scene.objects[1].w2l.sC, scene.objects[1].w2l.sD, scene.objects[1].w2l.sE, scene.objects[1].w2l.sF);

	alloc_cl_buffers(&cl_context, &scene);
	set_kernel_args(&cl_context, &scene);
 
	size_t global_item_size = scene.im_width * scene.im_height;
	// size_t local_item_size = 64;
			
	/* Execute OpenCL kernel as data parallel */
	ret = clEnqueueNDRangeKernel(cl_context.command_queue, cl_context.kernel, 1, NULL, 
		&global_item_size, NULL, 0, NULL, NULL);
 
	/* Transfer result to host */
	ret = clEnqueueReadBuffer(cl_context.command_queue, cl_context.pixels_buf, CL_TRUE, 0,
		scene.im_width * scene.im_height * sizeof(cl_float3), pixels, 0, NULL, NULL);

	if (ret != CL_SUCCESS)
		exit(-1);

	// save_image(pixels);

	// ---------- SDL part ----------------------------
	for (int i = 0; i < scene.im_width * scene.im_height; i++)
	{
		sdl_context.data[i * 4] = to_uchar(pixels[i].s[2]);
		sdl_context.data[i * 4 + 1] = to_uchar(pixels[i].s[1]);
		sdl_context.data[i * 4 + 2] = to_uchar(pixels[i].s[0]);
		sdl_context.data[i * 4 + 3] = 1;
	}

	sdl_context.surf = SDL_CreateRGBSurfaceFrom(sdl_context.data,
		scene.im_width, scene.im_height, 32, scene.im_width * 4, 0, 0, 0, 0);
	sdl_context.tex = SDL_CreateTextureFromSurface(sdl_context.rend, sdl_context.surf);

	int to_close = 0;
	
	while (!to_close)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:	to_close = 1; break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE: to_close = 1; break;
					}
			}
		}
		SDL_RenderClear(sdl_context.rend);
		SDL_RenderCopy(sdl_context.rend, sdl_context.tex, NULL, NULL);
		SDL_RenderPresent(sdl_context.rend);
	}

	SDL_FreeSurface(sdl_context.surf);
	SDL_DestroyTexture(sdl_context.tex);

	sdl_cleanup(&sdl_context);
	// ---------------------------------------------------

	cl_cleanup(&cl_context);

	free(pixels);
	free(scene.objects);
	free(scene.lights);
	free(scene.cam);
 
	return (0);
}	
