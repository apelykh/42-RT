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

cJSON	*cj_obj(cJSON *root, char *item_name)
{
	return (cJSON_GetObjectItem(root, item_name));
}

void	cj_get_bool(cl_bool *target, cJSON *object, char *item_name)
{
	cJSON	*bool_obj;

	bool_obj = cJSON_GetObjectItem(object, item_name);
	if (bool_obj)
	{
		if (!cJSON_IsBool(bool_obj))
			ft_error("[-] Parsing: Error Parsing Boolean", item_name);
		*target = (cl_bool)bool_obj->valueint;
	}
}

char	*cj_get_str(cJSON *object, char *item_name)
{
	cJSON	*string_obj;
	char	*res;

	res = "";
	string_obj = cJSON_GetObjectItem(object, item_name);
	if (string_obj)
	{
		if (!cJSON_IsString(string_obj))
			ft_error("[-] Parsing: Error Parsing String", item_name);
		res = string_obj->valuestring;
	}
	return (res);
}

cl_int	cj_get_obj_type(char *string_type)
{
	cl_int	result;

	if ((result = -1) && !strcmp(string_type, "PLANE"))
		result = 0;
	else if (!strcmp(string_type, "SPHERE"))
		result = 1;
	else if (!strcmp(string_type, "CYLINDER"))
		result = 2;
	else if (!strcmp(string_type, "CONE"))
		result = 3;
	else if (!strcmp(string_type, "BOX"))
		result = 4;
	else if (!strcmp(string_type, "UNION"))
		result = 5;
	else if (!strcmp(string_type, "INTERSECTION"))
		result = 6;
	else if (!strcmp(string_type, "DIFFERENCE"))
		result = 7;
	else if (!strcmp(string_type, "CLIPPING"))
		result = 8;
	else if (!strcmp(string_type, "BOCAL"))
		result = 9;
	else
		ft_error("[-] Parsing: Error Parsing Object Type", string_type);
	return (result);
}

cl_int	cj_get_light_type(char *string_type)
{
	cl_int	result;

	result = -1;
	if (!strcmp(string_type, "AMBIENT"))
		result = 0;
	else if (!strcmp(string_type, "POINT"))
		result = 1;
	else if (!strcmp(string_type, "PARALLEL"))
		result = 2;
	else if (!strcmp(string_type, "SPOT"))
		result = 3;
	else
		ft_error("[-] Parsing: Error Parsing Light Type", string_type);
	return (result);
}
