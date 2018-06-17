/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/14 23:28:58 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 19:14:01 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static float	minmax_float(float x, float min, float max)
{
	if (x < min || x > max)
		ft_error("[-] Float value outside the limit.", NULL);
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

int				count_inner_objects(cJSON *cj_objects, int count_objects)
{
	cJSON	*cj_inner_objects;
	cJSON	*cj_current_obj;
	int		type;
	int		count_inner;
	int		i;

	i = 0;
	count_inner = 0;
	while (i < count_objects)
	{
		cj_current_obj = cJSON_GetArrayItem(cj_objects, i);
		type = cj_get_obj_type(cj_get_str(cj_current_obj, "type"));
		if (type >= UNION && type <= CLIPPING)
		{
			cj_inner_objects = cJSON_GetObjectItem(
					cj_current_obj, "inner_objects");
			count_inner += cJSON_GetArraySize(cj_inner_objects);
		}
		i++;
	}
	return (count_inner);
}
