#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
// #include <SDL2/SDL_timer.h>
// #include <SDL2/SDL_image.h>

#include "rt.h"
 
#ifdef __APPLE__
# include <OpenCL/opencl.h>
#else
# include <CL/cl.h>
#endif
 
#define MAX_SOURCE_SIZE (0x100000)

void	sdl_cleanup(t_sdl_context *sdl_context)
{
	if (sdl_context->win)
		SDL_DestroyWindow(sdl_context->win);

	if (sdl_context->rend)
		SDL_DestroyRenderer(sdl_context->rend);

	free(sdl_context->data);

	SDL_Quit();
	printf("[+] SDL resources freed\n");
}

void	init_sdl(t_sdl_context *sdl_context)
{
	Uint32 render_flags;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("SDL Init error: %s\n", SDL_GetError());
		exit(-1);
	}

	sdl_context->win = SDL_CreateWindow("rt-dev", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
	if (!sdl_context->win)
	{
		printf("[-] Error creating window: %s\n", SDL_GetError());
		sdl_cleanup(sdl_context);
		exit(-1);
	}

	render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	sdl_context->rend = SDL_CreateRenderer(sdl_context->win, -1, render_flags);
	if (!sdl_context->rend)
	{
		printf("[-] Error creating renderer: %s\n", SDL_GetError());
		sdl_cleanup(sdl_context);
		exit(-1);
	}

	sdl_context->data = (unsigned char*)malloc(4 * WIN_WIDTH * WIN_HEIGHT);

	printf("[+] SDL Init success\n");
}

void	load_kernel(t_cl_context *cl_context)
{
	char filename[] = "./kernels/path_tracing.cl";
	FILE *fp;

	fp = fopen(filename, "r");
	if (!fp)
	{
		fprintf(stderr, "[-] Failed to load kernel.\n");
		exit(1);
	}

	cl_context->kernel_src = (char*)malloc(MAX_SOURCE_SIZE);
	cl_context->src_size = fread(cl_context->kernel_src, 1, MAX_SOURCE_SIZE, fp);

	fclose(fp);
}

void	init_cl(t_cl_context *cl_context)
{
	cl_platform_id	platform_id;
	cl_uint			num_platforms;
	cl_device_id	device_id;
	cl_uint			num_devices;
	cl_int			ret;

	ret = clGetPlatformIDs(1, &platform_id, &num_platforms);	
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);
 
	cl_context->context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
	cl_context->command_queue = clCreateCommandQueue(cl_context->context, device_id, 0, &ret);

	load_kernel(cl_context);

	cl_context->program = clCreateProgramWithSource(cl_context->context, 1,
		(const char **)&(cl_context->kernel_src), (const size_t *)&(cl_context->src_size), &ret);

	ret = clBuildProgram(cl_context->program, 1, &device_id, NULL, NULL, NULL);
	if (ret != CL_SUCCESS)
	{
		char *buff_erro;
		cl_int errcode;
		size_t build_log_len;

		errcode = clGetProgramBuildInfo(cl_context->program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_len);
		if (errcode)
		{
			printf("clGetProgramBuildInfo failed at line %d\n", __LINE__);
			exit(-1);
		}

	    buff_erro = (char*)malloc(build_log_len);
	    if (!buff_erro)
	    {
	        printf("malloc failed at line %d\n", __LINE__);
	        exit(-2);
	    }

	    errcode = clGetProgramBuildInfo(cl_context->program, device_id, CL_PROGRAM_BUILD_LOG, build_log_len, buff_erro, NULL);
	    if (errcode)
	    {
	        printf("clGetProgramBuildInfo failed at line %d\n", __LINE__);
	        exit(-3);
	    }

	    fprintf(stderr, "Build log: \n%s\n", buff_erro); //Be careful with  the fprint
	    free(buff_erro);
	    fprintf(stderr,"clBuildProgram failed\n");
	    exit(EXIT_FAILURE);
	}

	cl_context->kernel = clCreateKernel(cl_context->program, "render_kernel", &ret);
}

void	cl_cleanup(t_cl_context *cl_context)
{
	cl_int ret;

	ret = clFlush(cl_context->command_queue);
	ret = clFinish(cl_context->command_queue);
	ret = clReleaseKernel(cl_context->kernel);
	ret = clReleaseProgram(cl_context->program);
	ret = clReleaseCommandQueue(cl_context->command_queue);
	ret = clReleaseContext(cl_context->context);

	free(cl_context->kernel_src);
}

float	clamp(float x)
{
	return x < 0.0f ? 0.0f : x > 1.0f ? 1.0f : x;
}

int		to_uchar(float x)
{
	return (unsigned char)(clamp(x) * 255 + .5);
}

int		to_int(float x)
{
	return (int)(clamp(x) * 255 + .5);
}

void	save_image(cl_float3 *pixels)
{
	// write image to PPM file, a very simple image file format
	// PPM files can be opened with IrfanView (download at www.irfanview.com) or GIMP
	FILE *f = fopen("./saved_images/9_spheres.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", WIN_WIDTH, WIN_HEIGHT, 255);

	// loop over all pixels, write RGB values
	for (int i = 0; i < WIN_WIDTH * WIN_HEIGHT; i++)
		fprintf(f, "%d %d %d ",
		to_int(pixels[i].s[0]),
		to_int(pixels[i].s[1]),
		to_int(pixels[i].s[2]));

	printf("[+] Image saved!\n");
}

void	init_scene(t_sphere *cpu_spheres)
{
	// left wall
	// cpu_spheres[0].radius = 200.0f;
	// cpu_spheres[0].pos.x = -200.6f;
	// cpu_spheres[0].pos.y = 0.0f;
	// cpu_spheres[0].pos.z = 0.0f;
	// cpu_spheres[0].color.x = 0.75f;
	// cpu_spheres[0].color.y = 0.25f;
	// cpu_spheres[0].color.z = 0.25f;
	// cpu_spheres[0].emi.x = 0.0f;
	// cpu_spheres[0].emi.y = 0.0f;
	// cpu_spheres[0].emi.z = 0.0f;

	// right wall
	// cpu_spheres[1].radius = 200.0f;
	// cpu_spheres[1].pos.x = 200.6f;
	// cpu_spheres[1].pos.y = 0.0f;
	// cpu_spheres[1].pos.z = 0.0f;
	// cpu_spheres[1].color.x = 0.25f;
	// cpu_spheres[1].color.y = 0.25f;
	// cpu_spheres[1].color.z = 0.75f;
	// cpu_spheres[1].emi.x = 0.0f;
	// cpu_spheres[1].emi.y = 0.0f;
	// cpu_spheres[1].emi.z = 0.0f;

	// floor
	cpu_spheres[0].radius = 200.0f;
	cpu_spheres[0].pos.x = 0.0f;
	cpu_spheres[0].pos.y = -200.4f;
	cpu_spheres[0].pos.z = 0.0f;
	cpu_spheres[0].color.x = 0.9f;
	cpu_spheres[0].color.y = 0.8f;
	cpu_spheres[0].color.z = 0.7f;
	cpu_spheres[0].emi.x = 0.0f;
	cpu_spheres[0].emi.y = 0.0f;
	cpu_spheres[0].emi.z = 0.0f;

	// ceiling
	cpu_spheres[1].radius = 200.0f;
	cpu_spheres[1].pos.x = 0.0f;
	cpu_spheres[1].pos.y = 200.4f;
	cpu_spheres[1].pos.z = 0.0f;
	cpu_spheres[1].color.x = 0.9f;
	cpu_spheres[1].color.y = 0.8f;
	cpu_spheres[1].color.z = 0.7f;
	cpu_spheres[1].emi.x = 0.0f;
	cpu_spheres[1].emi.y = 0.0f;
	cpu_spheres[1].emi.z = 0.0f;

	// back wall
	cpu_spheres[2].radius = 200.0f;
	cpu_spheres[2].pos.x = 0.0f;
	cpu_spheres[2].pos.y = 0.0f;
	cpu_spheres[2].pos.z = -201.0f;
	cpu_spheres[2].color.x = 0.9f;
	cpu_spheres[2].color.y = 0.8f;
	cpu_spheres[2].color.z = 0.7f;
	cpu_spheres[2].emi.x = 0.0f;
	cpu_spheres[2].emi.y = 0.0f;
	cpu_spheres[2].emi.z = 0.0f;

	// // front wall 
	// cpu_spheres[5].radius = 200.0f;
	// cpu_spheres[5].pos.x = 0.0f;
	// cpu_spheres[5].pos.y = 0.0f;
	// cpu_spheres[5].pos.z = 202.0f;
	// cpu_spheres[5].color.x = 0.9f;
	// cpu_spheres[5].color.y = 0.8f;
	// cpu_spheres[5].color.z = 0.7f;
	// cpu_spheres[5].emi.x = 0.0f;
	// cpu_spheres[5].emi.y = 0.0f;
	// cpu_spheres[5].emi.z = 0.0f;

	// left sphere
	cpu_spheres[3].radius = 0.16f;
	cpu_spheres[3].pos.x = -0.25f;
	cpu_spheres[3].pos.y = -0.24f;
	cpu_spheres[3].pos.z = -0.1f;
	cpu_spheres[3].color.x = 0.5f;
	cpu_spheres[3].color.y = 0.6f;
	cpu_spheres[3].color.z = 0.7f;
	cpu_spheres[3].emi.x = 0.0f;
	cpu_spheres[3].emi.y = 0.0f;
	cpu_spheres[3].emi.z = 0.0f;

	// right sphere
	cpu_spheres[4].radius = 0.16f;
	cpu_spheres[4].pos.x = 0.25f;
	cpu_spheres[4].pos.y = -0.35f;
	cpu_spheres[4].pos.z = 0.1f;
	cpu_spheres[4].color.x = 0.5f;
	cpu_spheres[4].color.y = 0.6f;
	cpu_spheres[4].color.z = 0.7f;
	cpu_spheres[4].emi.x = 0.0f;
	cpu_spheres[4].emi.y = 0.0f;
	cpu_spheres[4].emi.z = 0.0f;

	// lightsource
	cpu_spheres[5].radius = 0.08f;
	cpu_spheres[5].pos.x = 0.0f;
	cpu_spheres[5].pos.y = 0.25f;
	cpu_spheres[5].pos.z = 0.5f;
	cpu_spheres[5].color.x = 0.7f;
	cpu_spheres[5].color.y = 0.2f;
	cpu_spheres[5].color.z = 0.4f;
	cpu_spheres[5].emi.x = 0.75f;
	cpu_spheres[5].emi.y = 0.75f;
	cpu_spheres[5].emi.z = 0.75f;
}

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
	const int sphere_count = 6;
	t_sphere cpu_spheres[sphere_count];
	init_scene(cpu_spheres);

	pixels_buf = clCreateBuffer(cl_context.context, CL_MEM_WRITE_ONLY,
		im_width * im_height * sizeof(cl_float3), NULL, &ret);
	spheres_buf = clCreateBuffer(cl_context.context, CL_MEM_READ_ONLY,
		sphere_count * sizeof(t_sphere), NULL, &ret);

	// white shperes (objects) array to the device
	ret = clEnqueueWriteBuffer(cl_context.command_queue, spheres_buf, CL_TRUE, 0,
		sphere_count * sizeof(t_sphere), &cpu_spheres, 0, NULL, NULL);

	// ------------ LIGHTS -------------------------
	const int num_lights = 1;
	t_light cpu_lights[num_lights];

	cpu_lights[0].pos.x = 0.2f;
	cpu_lights[0].pos.y = 0.2f;
	cpu_lights[0].pos.z = 0.0f;
	cpu_lights[0].emi.x = 0.8f;
	cpu_lights[0].emi.y = 0.8f;
	cpu_lights[0].emi.z = 0.8f;

	cpu_lights[1].pos.x = 0.2f;
	cpu_lights[1].pos.y = 0.2f;
	cpu_lights[1].pos.z = 0.4f;
	cpu_lights[1].emi.x = 0.8f;
	cpu_lights[1].emi.y = 0.8f;
	cpu_lights[1].emi.z = 0.8f;

	lights_buf = clCreateBuffer(cl_context.context, CL_MEM_READ_ONLY,
		num_lights * sizeof(t_light), NULL, &ret);

	// white lights array to the device
	ret = clEnqueueWriteBuffer(cl_context.command_queue, lights_buf, CL_TRUE, 0,
		num_lights * sizeof(t_light), &cpu_lights, 0, NULL, NULL);

	ret = clSetKernelArg(cl_context.kernel, 0, sizeof(cl_mem), (void *)&spheres_buf);
	ret = clSetKernelArg(cl_context.kernel, 1, sizeof(int), (void *)&sphere_count);
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

	save_image(pixels);

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

	ret = clReleaseMemObject(pixels_buf);
	cl_cleanup(&cl_context);

	free(pixels);
 
	return (0);
}	
