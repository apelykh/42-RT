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

	alloc_cl_buffers(&cl_context, &scene);
	set_kernel_args(&cl_context, &scene);
 
	size_t global_item_size = scene.im_width * scene.im_height;

	ret = clEnqueueNDRangeKernel(cl_context.command_queue, cl_context.kernel, 1, NULL, 
		&global_item_size, NULL, 0, NULL, NULL);
 
	/* Transfer result to host */
	ret = clEnqueueReadBuffer(cl_context.command_queue, cl_context.pixels_buf, CL_TRUE, 0,
		scene.im_width * scene.im_height * sizeof(cl_float3), pixels, 0, NULL, NULL);

	if (ret != CL_SUCCESS)
		exit(-1);

	// save_image(pixels);

	// ---------- SDL part ----------------------------
	// for (int i = 0; i < scene.im_width * scene.im_height; i++)
	// {
	// 	sdl_context.data[i * 4] = to_uchar(pixels[i].s[2]);
	// 	sdl_context.data[i * 4 + 1] = to_uchar(pixels[i].s[1]);
	// 	sdl_context.data[i * 4 + 2] = to_uchar(pixels[i].s[0]);
	// 	sdl_context.data[i * 4 + 3] = 1;
	// }

	// sdl_context.surf = SDL_CreateRGBSurfaceFrom(sdl_context.data,
	// 	scene.im_width, scene.im_height, 32, scene.im_width * 4, 0, 0, 0, 0);
	// sdl_context.tex = SDL_CreateTextureFromSurface(sdl_context.rend, sdl_context.surf);

	SDL_Event	event;
	int			to_close;
	cl_float4	vec;

	to_close = 0;
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
					vec = mat_mult_vec(scene.cam->rot_matrix, init_vec4(0.f, 0.f, -.2f, 0.f));

					scene.cam->loc = add_vec3(scene.cam->loc, vec);
					set_cam_transl_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					vec = mat_mult_vec(scene.cam->rot_matrix, init_vec4(0.f, 0.f, .2f, 0.f));

					scene.cam->loc = add_vec3(scene.cam->loc, vec);
					set_cam_transl_matrix(scene.cam);
				}

				if (event.key.keysym.scancode == SDL_SCANCODE_A)
				{
					vec = mat_mult_vec(scene.cam->rot_matrix, init_vec4(-.2f, 0.f, 0.0f, 0.f));

					scene.cam->loc = add_vec3(scene.cam->loc, vec);
					set_cam_transl_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_D)
				{
					vec = mat_mult_vec(scene.cam->rot_matrix, init_vec4(.2f, 0.f, 0.0f, 0.f));
					printf("move vector %f %f %f\n", vec.x, vec.y, vec.z);
					printf("cur loc %f %f %f\n", scene.cam->loc.x, scene.cam->loc.y, scene.cam->loc.z );
					scene.cam->loc = add_vec3(scene.cam->loc, vec);
					printf("new loc %f %f %f\n", scene.cam->loc.x, scene.cam->loc.y, scene.cam->loc.z );
					set_cam_transl_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					scene.cam->rot.y += 5.f;
					set_cam_rot_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					scene.cam->rot.y -= 5.f;
					set_cam_rot_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					scene.cam->rot.x += 5.f;
					set_cam_rot_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					scene.cam->rot.x -= 5.f;
					set_cam_rot_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_PAGEUP)
				{
					scene.cam->rot.z += 5.f;
					set_cam_rot_matrix(scene.cam);
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_PAGEDOWN)
				{
					scene.cam->rot.z -= 5.f;
					set_cam_rot_matrix(scene.cam);
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
			scene.im_width * scene.im_height * sizeof(cl_float3), pixels, 0, NULL, NULL);

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

		SDL_RenderClear(sdl_context.rend);
		SDL_RenderCopy(sdl_context.rend, sdl_context.tex, NULL, NULL);
		SDL_RenderPresent(sdl_context.rend);

		SDL_FreeSurface(sdl_context.surf);
		SDL_DestroyTexture(sdl_context.tex);
	}
	// SDL_FreeSurface(sdl_context.surf);
	// SDL_DestroyTexture(sdl_context.tex);

	sdl_cleanup(&sdl_context);
	// ---------------------------------------------------

	cl_cleanup(&cl_context);

	free(pixels);
	free(scene.objects);
	free(scene.lights);
	free(scene.cam);
 
	return (0);
}	
