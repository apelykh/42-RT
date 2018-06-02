#include "rt.h"

void		parsing_error(char *text1, char *text2)
{
	ft_putstr(text1);
	ft_putstr("\n");
	if (text2 != NULL) {
		ft_putstr(text2);
		ft_putstr("\n");
	}
	exit(EXIT_FAILURE);
}

cl_float3	cjGetFloat3(cJSON *root, char *item_name)
{
	cJSON		*j_float3_arr;
	cl_float3	res;

	res = init_vec3(0.0f, 0.0f, 0.0f);
	j_float3_arr = cJSON_GetObjectItem(root, item_name);
	if (j_float3_arr)
	{
		if (cJSON_GetArraySize(j_float3_arr) != 3)
			parsing_error("[-] Error Parsing Float3", item_name);

		if (!cJSON_IsNumber(cJSON_GetArrayItem(j_float3_arr, 0))
				|| !cJSON_IsNumber(cJSON_GetArrayItem(j_float3_arr, 1))
				|| !cJSON_IsNumber(cJSON_GetArrayItem(j_float3_arr, 2)))
			parsing_error("[-] Error Parsing Float3", item_name);
		res.x = (cl_float)cJSON_GetArrayItem(j_float3_arr, 0)->valuedouble;
		res.y = (cl_float)cJSON_GetArrayItem(j_float3_arr, 1)->valuedouble;
		res.z = (cl_float)cJSON_GetArrayItem(j_float3_arr, 2)->valuedouble;
	}
	return (res);
}

cl_float	cjGetFloat(cJSON *object, char *item_name)
{
	cJSON		*float_obj;
	cl_float	res;

	res = 0.0f;
	float_obj = cJSON_GetObjectItem(object, item_name);
	if (float_obj)
	{
		if (!cJSON_IsNumber(float_obj))
			parsing_error("[-] Error Parsing Float", item_name);
		res = (cl_float)float_obj->valuedouble;
	}
	return (res);
}

void		cjGetBool(cl_bool *target, cJSON *object, char *item_name)
{
	cJSON	*bool_obj;

	bool_obj = cJSON_GetObjectItem(object, item_name);
	if (bool_obj)
	{
		if (!cJSON_IsBool(bool_obj))
			parsing_error("[-] Error Parsing Boolean", item_name);
		*target = (cl_bool)bool_obj->valueint;
	};
}

char		*cjGetString(cJSON *object, char *item_name)
{
	cJSON	*string_obj;
	char	*res;

	res = NULL;
	string_obj = cJSON_GetObjectItem(object, item_name);
	if (string_obj)
	{
		if (!cJSON_IsString(string_obj))
			parsing_error("[-] Error Parsing String", item_name);
		res = string_obj->valuestring;
	}
	return (res);
}

cl_int		cjGetType(char *string_type)
{
	cl_int	result;

	result = -1;
	if (!strcmp(string_type, "PLANE"))
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
		parsing_error("[-] Error Parsing Object Type", string_type);
	return (result);
}

cl_int		cjGetLightType(char *string_type)
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
		parsing_error("[-] Error Parsing Light Type", string_type);
	return (result);
}
