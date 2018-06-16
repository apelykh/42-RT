/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_camera.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 23:19:24 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/16 15:00:35 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	camera_init(t_camera *camera, cJSON *cj_camera)
{
	save_float3(&(camera->location),
				cj_obj(cj_camera, "location"), -100.0f, 100.0f);
	save_float3(&(camera->rotation),
				cj_obj(cj_camera, "rotation"), -180.0f, 180.0f);
	save_float(&(camera->fov),
				cj_obj(cj_camera, "fov"), 30.0f, 120.0f);
}

void		parse_camera(cJSON *cj_root, t_scene *scene)
{
	cJSON	*cj_camera;

	if (!(cj_camera = cJSON_GetObjectItem(cj_root, "camera")))
		ft_error("[-] Parsing: No camera field in scene document", NULL);
	scene->cam = (t_camera*)malloc(sizeof(t_camera));
	camera_init_empty(scene->cam);
	camera_init(scene->cam, cj_camera);
	set_cam_translate_matrix(scene->cam);
	set_cam_rotate_matrix(scene->cam);
}
