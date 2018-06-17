/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/14 23:28:58 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 17:05:15 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static float	minmax_float(float x, float min, float max)
{
	if (x < min || x > max)
		ft_error("-] Float value outside the limit.", NULL);
	return (x);
}

static cl_float	cj_get_arr_float(cJSON *array, cl_int index)
{
	return (cl_float)(cJSON_GetArrayItem(array, index)->valuedouble);
}

void			save_float3(cl_float3 *target, cJSON *cj_array,
							float min, float max)
{
	if (cj_array)
	{
		if (cJSON_GetArraySize(cj_array) != 3)
			ft_error("[-] Parsing: Error Parsing Float3",
						cj_array->string);
		if (!cJSON_IsNumber(cJSON_GetArrayItem(cj_array, 0))
			|| !cJSON_IsNumber(cJSON_GetArrayItem(cj_array, 1))
			|| !cJSON_IsNumber(cJSON_GetArrayItem(cj_array, 2)))
			ft_error("[-] Parsing: Error Parsing Float3",
						cj_array->string);
		(*target).x = (cl_float)minmax_float(
				cj_get_arr_float(cj_array, 0), min, max);
		(*target).y = (cl_float)minmax_float(
				cj_get_arr_float(cj_array, 1), min, max);
		(*target).z = (cl_float)minmax_float(
				cj_get_arr_float(cj_array, 2), min, max);
	}
	else
	{
		(*target).x = (cl_float)minmax_float((*target).x, min, max);
		(*target).y = (cl_float)minmax_float((*target).y, min, max);
		(*target).z = (cl_float)minmax_float((*target).z, min, max);
	}
}

void			save_float(cl_float *target, cJSON *float_obj,
							float min, float max)
{
	if (float_obj)
	{
		if (!cJSON_IsNumber(float_obj))
			ft_error("[-] Parsing: Error Parsing Float",
						float_obj->string);
		*target = (cl_float)minmax_float(float_obj->valuedouble, min, max);
	}
	else
		*target = (cl_float)minmax_float(*target, min, max);
}
