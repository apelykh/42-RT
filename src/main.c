#include <stdio.h>
#include <stdlib.h>
#include "rt.h"


int		main(void)
{
	t_cl_context cl_context;
	t_sdl_context sdl_context;

	int im_width = WIN_WIDTH;
	int im_height = WIN_HEIGHT;

	cl_mem pixels_buf = NULL;
	cl_mem spheres_buf = NULL;
	cl_mem lights_buf = NULL;
	cl_int ret;
 
	cl_float3 *pixels;
	pixels = (cl_float3*)malloc(im_width * im_height * sizeof(cl_float3));
 
	init_cl(&cl_context);
	init_sdl(&sdl_context);

	// ------------ OBJECTS -------------------------
	const int num_objs = 5;
	t_object cpu_objects[num_objs];
	init_objects(cpu_objects);

	pixels_buf = clCreateBuffer(cl_context.context, CL_MEM_WRITE_ONLY,
		im_width * im_height * sizeof(cl_float3), NULL, &ret);
	spheres_buf = clCreateBuffer(cl_context.context, CL_MEM_READ_ONLY,
		num_objs * sizeof(t_object), NULL, &ret);

	// white shperes (objects) array to the device
	ret = clEnqueueWriteBuffer(cl_context.command_queue, spheres_buf, CL_TRUE, 0,
		num_objs * sizeof(t_object), &cpu_objects, 0, NULL, NULL);

	// ------------ LIGHTS -------------------------
	const int num_lights = 2;
	t_light cpu_lights[num_lights];
	cpu_lights[0].pos = init_vec3(1.0f, 0.6f, 1.0f);
	cpu_lights[0].emi = init_vec3(0.7f, 0.7f, 0.7f);

	cpu_lights[1].pos = init_vec3(3.0f, 5.0f, -7.0f);
	cpu_lights[1].emi = init_vec3(0.8f, 0.8f, 0.8f);

	// cpu_lights[2].pos = init_vec3(0.2f, 0.1f, 0.5f);
	// cpu_lights[2].emi = init_vec3(0.9f, 0.9f, 0.9f);

	lights_buf = clCreateBuffer(cl_context.context, CL_MEM_READ_ONLY,
		num_lights * sizeof(t_light), NULL, &ret);

	// white lights array to the device
	ret = clEnqueueWriteBuffer(cl_context.command_queue, lights_buf, CL_TRUE, 0,
		num_lights * sizeof(t_light), &cpu_lights, 0, NULL, NULL);

	ret = clSetKernelArg(cl_context.kernel, 0, sizeof(cl_mem), (void *)&spheres_buf);
	ret = clSetKernelArg(cl_context.kernel, 1, sizeof(int), (void *)&num_objs);
	ret = clSetKernelArg(cl_context.kernel, 2, sizeof(cl_mem), (void *)&lights_buf);
	ret = clSetKernelArg(cl_context.kernel, 3, sizeof(int), (void *)&num_lights);
	ret = clSetKernelArg(cl_context.kernel, 4, sizeof(cl_mem), (void *)&pixels_buf);
	ret = clSetKernelArg(cl_context.kernel, 5, sizeof(int), (void *)&im_width);
	ret = clSetKernelArg(cl_context.kernel, 6, sizeof(int), (void *)&im_height);
 
	size_t global_item_size = im_width * im_height;
	size_t local_item_size = 64;
			
	/* Execute OpenCL kernel as data parallel */
	ret = clEnqueueNDRangeKernel(cl_context.command_queue, cl_context.kernel, 1, NULL, 
		&global_item_size, &local_item_size, 0, NULL, NULL);
 
	/* Transfer result to host */
	ret = clEnqueueReadBuffer(cl_context.command_queue, pixels_buf, CL_TRUE, 0,
		im_width * im_height * sizeof(cl_float3), pixels, 0, NULL, NULL);

	// save_image(pixels);

	// ---------- SDL part ----------------------------
	for (int i = 0; i < im_width * im_height; i++)
	{
		sdl_context.data[i * 4] = to_uchar(pixels[i].s[0]);
		sdl_context.data[i * 4 + 1] = to_uchar(pixels[i].s[1]);
		sdl_context.data[i * 4 + 2] = to_uchar(pixels[i].s[2]);
		sdl_context.data[i * 4 + 3] = 1;
	}

	sdl_context.surf = SDL_CreateRGBSurfaceFrom(sdl_context.data,
		im_width, im_height, 32, im_width * 4, 0, 0, 0, 0);
	sdl_context.tex = SDL_CreateTextureFromSurface(sdl_context.rend, sdl_context.surf);

	int to_close = 0;
	
	while (!to_close)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				to_close = 1;
		}
		SDL_RenderClear(sdl_context.rend);
		SDL_RenderCopy(sdl_context.rend, sdl_context.tex, NULL, NULL);
		SDL_RenderPresent(sdl_context.rend);
	}

	SDL_FreeSurface(sdl_context.surf);
	SDL_DestroyTexture(sdl_context.tex);

	sdl_cleanup(&sdl_context);
	// ---------------------------------------------------

	ret = clReleaseMemObject(pixels_buf);
	ret = clReleaseMemObject(spheres_buf);
	ret = clReleaseMemObject(lights_buf);
	cl_cleanup(&cl_context);

	free(pixels);
 
	return (0);
}	
