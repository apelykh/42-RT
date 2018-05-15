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

	init_cl(&cl_context);
	init_sdl(&sdl_context);

	// init_scene1(&scene);

	parse_scene("./scenes/test.json", &scene);

	alloc_cl_buffers(&cl_context, &scene);
	set_kernel_args(&cl_context, &scene);
 
	size_t global_item_size = scene.im_width * scene.im_height;

	SDL_Event	event;
	cl_float4	vec;
	int			to_close = 0;

	while (!to_close)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				to_close = 1;
			if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					to_close = 1;
				if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(0.f, 0.f, -.2f, 0.f));

					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(0.f, 0.f, .2f, 0.f));

					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_Q)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(0.f, -.2f, 0.f, 0.f));

					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_E)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(0.f, .2f, 0.f, 0.f));

					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				} 
				if (event.key.keysym.scancode == SDL_SCANCODE_A)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(-.2f, 0.f, 0.0f, 0.f));

					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_D)
				{
					vec = mat_mult_vec(scene.cam->rotate_matrix, init_vec4(.2f, 0.f, 0.0f, 0.f));
					scene.cam->location = add_vec3(scene.cam->location, vec);
					set_cam_translate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					scene.cam->rotation.y += 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					scene.cam->rotation.y -= 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					scene.cam->rotation.x += 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					scene.cam->rotation.x -= 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_PAGEUP)
				{
					scene.cam->rotation.z += 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_PAGEDOWN)
				{
					scene.cam->rotation.z -= 5.f;
					set_cam_rotate_matrix(scene.cam);
				}
			}
		}

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
