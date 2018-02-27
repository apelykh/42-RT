#include <stdio.h>
#include "rt.h"


static void	load_kernel(t_cl_context *cl_context)
{
	char filename[] = "./kernels/rt.cl";
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

	    //Be careful with the fprint
	    fprintf(stderr, "Build log: \n%s\n", buff_erro);
	    free(buff_erro);
	    fprintf(stderr,"clBuildProgram failed\n");
	    exit(EXIT_FAILURE);
	}

	cl_context->kernel = clCreateKernel(cl_context->program, "main", &ret);
}

void	cl_cleanup(t_cl_context *cl_context)
{
	clFlush(cl_context->command_queue);
	clFinish(cl_context->command_queue);
	clReleaseKernel(cl_context->kernel);
	clReleaseProgram(cl_context->program);
	clReleaseCommandQueue(cl_context->command_queue);
	clReleaseContext(cl_context->context);

	free(cl_context->kernel_src);
}
