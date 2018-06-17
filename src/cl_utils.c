/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 15:58:37 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 16:23:44 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	load_kernel(t_cl_context *cl_context)
{
	int		fd;

	if ((fd = open("./kernels/render.cl", O_RDONLY)) < 0 || read(fd, 0, 0) < 0)
	{
		close(fd);
		ft_error("[-] Failed to load kernel from file.", "");
	}
	cl_context->kernel_src = (char*)malloc(MAX_SRC_SIZE);
	cl_context->src_size = read(fd, cl_context->kernel_src, MAX_SRC_SIZE);
	close(fd);
}

void		cl_init(t_cl_context *cl_context)
{
	cl_platform_id	platform_id;
	cl_uint			num_platforms;
	cl_device_id	device_id;
	cl_uint			num_devices;
	cl_int			ret;

	ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id,
		&num_devices);
	cl_context->context = clCreateContext(NULL, 1, &device_id,
		NULL, NULL, &ret);
	cl_context->command_queue = clCreateCommandQueue(cl_context->context,
		device_id, 0, &ret);
	load_kernel(cl_context);
	cl_context->program = clCreateProgramWithSource(cl_context->context, 1,
		(const char **)&(cl_context->kernel_src),
		(const size_t *)&(cl_context->src_size), &ret);
	ret = clBuildProgram(cl_context->program, 1, &device_id,
		"-I includes/ -I kernels/ -Werror", NULL, NULL);
	if (ret != CL_SUCCESS)
		ft_error("[-] clBuildProgram failed.", NULL);
	cl_context->kernel = clCreateKernel(cl_context->program,
		"render_scene", &ret);
}

void		cl_alloc_buffers(t_cl_context *cl_context, t_scene *scene)
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
	if (ret != CL_SUCCESS)
		ft_error("[-] OpenCL objects buffer allocation error.", NULL);
	ret = clEnqueueWriteBuffer(cl_context->command_queue,
		cl_context->lights_buf, CL_TRUE, 0,
		scene->num_lights * sizeof(t_light), scene->lights, 0, NULL, NULL);
	if (ret != CL_SUCCESS)
		ft_error("[-] OpenCL lights buffer allocation error.", NULL);
	ret = clEnqueueWriteBuffer(cl_context->command_queue, cl_context->cam_buf,
		CL_TRUE, 0, sizeof(t_camera), scene->cam, 0, NULL, NULL);
	if (ret != CL_SUCCESS)
		ft_error("[-] OpenCL camera buffer allocation error.", NULL);
}
