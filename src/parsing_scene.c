/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_scene.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 20:32:36 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/16 21:26:27 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	scene_init(char *str_json, t_scene *scene)
{
	cJSON	*cj_root;

	if (!(cj_root = cJSON_Parse(str_json)))
		ft_error("[-] Parsing: JSON Error", (char *)cJSON_GetErrorPtr());
	scene->im_width = WIN_WIDTH;
	scene->im_height = WIN_HEIGHT;
	scene->sepia = 0;
	scene->to_save_image = 0;
	controls_init_empty(&scene->controls);
	parse_camera(cj_root, scene);
	parse_lights(cj_root, scene);
	parse_objects(cj_root, scene);
	cJSON_Delete(cj_root);
}

static void	parse_scene(char *scene_str, t_scene *scene)
{
	int		i;

	if (scene_str[0] != '{')
		ft_error("[-] Parsing: No first bracket", NULL);
	scene_init(scene_str, scene);
	i = 0;
	while (i < scene->num_objects)
		obj_transform_mats(&scene->objects[i++]);
	free(scene_str);
	scene_str = NULL;
}

void		open_scene(char *scene_path, t_scene *scene)
{
	char	*scene_str;
	int		fd;

	if ((fd = open(scene_path, O_RDONLY)) < 0 || read(fd, 0, 0) < 0)
	{
		close(fd);
		perror("[-] Read Scene Error");
		exit(EXIT_FAILURE);
	}
	scene_str = (char *)malloc(MAX_SOURCE_SIZE);
	read(fd, scene_str, MAX_SOURCE_SIZE);
	close(fd);
	parse_scene(scene_str, scene);
}
