/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efedoryc <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 20:31:52 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/07 20:31:54 by efedoryc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

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

int				is_complex_obj(cl_int object_type)
{
	return (object_type >= UNION && object_type <= CLIPPING);
}