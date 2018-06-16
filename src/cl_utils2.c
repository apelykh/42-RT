/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cl_utils2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 21:43:42 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 21:45:09 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void		cl_set_kernel_args(t_cl_context *cl_context, t_scene *scene)
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
	ret = clSetKernelArg(cl_context->kernel, 8, sizeof(cl_int),
		(void*)&scene->sepia);
	if (ret != CL_SUCCESS)
		ft_error("[-] OpenCL setting kernel arguments error.", NULL);
}

void		cl_update_pixels(t_scene *scene, t_sdl_context *sdl_context,
	t_cl_context *cl_context)
{
	size_t		global_item_size;
	cl_int		ret;

	ret = clEnqueueWriteBuffer(cl_context->command_queue,
		cl_context->cam_buf, CL_TRUE, 0, sizeof(t_camera),
		scene->cam, 0, NULL, NULL);
	ret = clSetKernelArg(cl_context->kernel, 7, sizeof(cl_mem),
		(void*)&cl_context->cam_buf);
	ret = clSetKernelArg(cl_context->kernel, 8, sizeof(cl_mem),
		(void*)&scene->sepia);
	global_item_size = scene->im_width * scene->im_height;
	ret = clEnqueueNDRangeKernel(cl_context->command_queue,
		cl_context->kernel, 1, NULL, &global_item_size,
		NULL, 0, NULL, NULL);
	ret = clEnqueueReadBuffer(cl_context->command_queue,
		cl_context->pixels_buf, CL_TRUE, 0,
		scene->im_width * scene->im_height * sizeof(cl_uchar4),
		sdl_context->pixels, 0, NULL, NULL);
	if (ret != CL_SUCCESS)
		ft_error("[-] Updating by event went wrong!", NULL);
}

void		cl_cleanup(t_cl_context *cl_context)
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
