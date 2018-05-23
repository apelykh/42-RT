#include <stdio.h>
#include "rt.h"

static void	load_kernel(t_cl_context *cl_context)
{
	char filename[] = "./kernels/render.cl";
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

	ret = clBuildProgram(cl_context->program, 1, &device_id, "-I includes/ -I kernels/ -Werror", NULL, NULL);
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

	    //Be careful with the fprint
	    fprintf(stderr, "Build log: \n%s\n", buff_erro);
	    free(buff_erro);
	    fprintf(stderr,"clBuildProgram failed\n");
	    exit(EXIT_FAILURE);
	}

	cl_context->kernel = clCreateKernel(cl_context->program, "render_scene", &ret);
}

void	alloc_cl_buffers(t_cl_context *cl_context, t_scene *scene)
{
	cl_int ret;

	cl_context->pixels_buf = clCreateBuffer(cl_context->context,
		CL_MEM_WRITE_ONLY,
		scene->im_width * scene->im_height * sizeof(cl_uchar4), NULL, &ret);

	cl_context->objects_buf = clCreateBuffer(cl_context->context,
		CL_MEM_READ_ONLY, scene->num_objects * sizeof(t_object), NULL, &ret);

	cl_context->lights_buf = clCreateBuffer(cl_context->context,
		CL_MEM_READ_ONLY, scene->num_lights * sizeof(t_light), NULL, &ret);

	cl_context->cam_buf = clCreateBuffer(cl_context->context, CL_MEM_READ_ONLY,
		sizeof(t_camera), NULL, &ret);

	ret = clEnqueueWriteBuffer(cl_context->command_queue,
		cl_context->objects_buf, CL_TRUE, 0,
		scene->num_objects * sizeof(t_object), scene->objects, 0, NULL, NULL);

	ret = clEnqueueWriteBuffer(cl_context->command_queue,
		cl_context->lights_buf, CL_TRUE, 0,
		scene->num_lights * sizeof(t_light), scene->lights, 0, NULL, NULL);

	ret = clEnqueueWriteBuffer(cl_context->command_queue, cl_context->cam_buf,
		CL_TRUE, 0, sizeof(t_camera), scene->cam, 0, NULL, NULL);

	if (ret != CL_SUCCESS)
		exit(-1);
}

void	set_kernel_args(t_cl_context *cl_context, t_scene *scene)
{
	cl_int ret;

	ret = clSetKernelArg(cl_context->kernel, 0, sizeof(cl_mem),
		(void*)&cl_context->objects_buf);
	ret = clSetKernelArg(cl_context->kernel, 1, sizeof(cl_int),
		(void*)&scene->num_objects);
	ret = clSetKernelArg(cl_context->kernel, 2, sizeof(cl_mem),
		(void*)&cl_context->lights_buf);
	ret = clSetKernelArg(cl_context->kernel, 3, sizeof(cl_int),
		(void*)&scene->num_lights);
	ret = clSetKernelArg(cl_context->kernel, 4, sizeof(cl_mem),
		(void*)&cl_context->pixels_buf);
	ret = clSetKernelArg(cl_context->kernel, 5, sizeof(cl_int),
		(void*)&scene->im_width);
	ret = clSetKernelArg(cl_context->kernel, 6, sizeof(cl_int),
		(void*)&scene->im_height);
	ret = clSetKernelArg(cl_context->kernel, 7, sizeof(cl_mem),
		(void*)&cl_context->cam_buf);

	if (ret != CL_SUCCESS)
		exit(-1);
}

void	cl_cleanup(t_cl_context *cl_context)
{
	clReleaseMemObject(cl_context->pixels_buf);
	clReleaseMemObject(cl_context->objects_buf);
	clReleaseMemObject(cl_context->lights_buf);
	clReleaseMemObject(cl_context->cam_buf);

	clFlush(cl_context->command_queue);
	clFinish(cl_context->command_queue);
	clReleaseKernel(cl_context->kernel);
	clReleaseProgram(cl_context->program);
	clReleaseCommandQueue(cl_context->command_queue);
	clReleaseContext(cl_context->context);

	free(cl_context->kernel_src);
}
