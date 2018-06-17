/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_lights.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 20:32:12 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/17 18:53:39 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	light_init(t_light *light, cJSON *cj_current)
{
	light->type = cj_get_light_type(cj_get_str(cj_current, "type"));
	save_float3(&(light->location),
				cj_obj(cj_current, "location"), -100.0f, 100.0f);
	save_float3(&(light->emission),
				cj_obj(cj_current, "emission"), 0.0f, 1.0f);
	save_float(&(light->angle),
				cj_obj(cj_current, "angle"), 1.0f, 90.0f);
	save_float3(&(light->dir),
				cj_obj(cj_current, "direction"), -1.0f, 1.0f);
}

void		parse_lights(cJSON *cj_root, t_scene *scene)
{
	cJSON	*cj_lights;
	cJSON	*cj_current_light;
	cl_int	i;
	cl_int	count_ambient;

	if (!(cj_lights = cJSON_GetObjectItem(cj_root, "lights")))
		ft_error("[-] Parsing: No lights field in scene document", NULL);
	count_ambient = 0;
	i = 0;
	scene->num_lights = (cl_int)cJSON_GetArraySize(cj_lights);
	if (scene->num_lights <= 0)
		ft_error("[-] Parsing: Lights array is empty.", NULL);
	scene->lights = (t_light *)malloc(sizeof(t_light) * scene->num_lights);
	while (i < scene->num_lights)
	{
		cj_current_light = cJSON_GetArrayItem(cj_lights, i);
		light_init_empty(&(scene->lights[i]));
		light_init(&(scene->lights[i]), cj_current_light);
		if (scene->lights[i].type == AMBIENT)
			count_ambient++;
		if (count_ambient > 1)
			ft_error("[-] Parsing: Ambient lights must be only one", NULL);
		i++;
	}
}
