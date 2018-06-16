/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bocal_components.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/07 23:32:41 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/16 13:48:57 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	bocal_init_objects1_inners(t_scene *scene)
{
	scene->objects[1].id = 1;
	scene->objects[1].type = SPHERE;
	scene->objects[1].operand = init_int2(-1, -1);
	scene->objects[1].scale = init_vec3(1.0f, 1.0f, 1.0f);
	scene->objects[1].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].color = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[1].capped = CL_TRUE;
	scene->objects[1].hidden = CL_TRUE;
	scene->objects[1].spec_pow = 0.0f;
	scene->objects[2].id = 2;
	scene->objects[2].type = PLANE;
	scene->objects[2].operand = init_int2(-1, -1);
	scene->objects[2].scale = init_vec3(1.0f, 1.0f, 1.0f);
	scene->objects[2].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[2].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[2].color = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[2].capped = CL_TRUE;
	scene->objects[2].hidden = CL_TRUE;
	scene->objects[2].spec_pow = 0.0f;
}

static void	bocal_init_objects1(t_scene *scene)
{
	scene->objects[0].id = 0;
	scene->objects[0].type = CLIPPING;
	scene->objects[0].operand = init_int2(1, 2);
	scene->objects[0].scale = init_vec3(1.0f, 2.0f, 1.0f);
	scene->objects[0].location = init_vec3(0.0f, 1.5f, 0.0f);
	scene->objects[0].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[0].color = init_vec3(0.99f, 0.66f, 0.33f);
	scene->objects[0].capped = CL_FALSE;
	scene->objects[0].hidden = CL_TRUE;
	scene->objects[0].spec_pow = 20.0f;
	bocal_init_objects1_inners(scene);
}

static void	bocal_init_objects2_inners(t_scene *scene)
{
	scene->objects[4].id = 4;
	scene->objects[4].type = CYLINDER;
	scene->objects[4].operand = init_int2(-1, -1);
	scene->objects[4].scale = init_vec3(0.1f, 1.05f, 0.1f);
	scene->objects[4].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[4].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[4].color = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[4].capped = CL_TRUE;
	scene->objects[4].hidden = CL_TRUE;
	scene->objects[4].spec_pow = 0.0f;
	scene->objects[5].id = 5;
	scene->objects[5].type = CONE;
	scene->objects[5].operand = init_int2(-1, -1);
	scene->objects[5].scale = init_vec3(1.0f, 0.2f, 1.0f);
	scene->objects[5].location = init_vec3(0.0f, -0.4f, 0.0f);
	scene->objects[5].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[5].color = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[5].capped = CL_TRUE;
	scene->objects[5].hidden = CL_TRUE;
	scene->objects[5].spec_pow = 0.0f;
}

static void	bocal_init_objects2(t_scene *scene)
{
	scene->objects[3].id = 3;
	scene->objects[3].type = UNION;
	scene->objects[3].operand = init_int2(4, 5);
	scene->objects[3].scale = init_vec3(1.0f, 1.0f, 1.0f);
	scene->objects[3].location = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[3].rotation = init_vec3(0.0f, 0.0f, 0.0f);
	scene->objects[3].color = init_vec3(0.99f, 0.66f, 0.33f);
	scene->objects[3].capped = CL_FALSE;
	scene->objects[3].hidden = CL_TRUE;
	scene->objects[3].spec_pow = 20.0f;
	bocal_init_objects2_inners(scene);
}

void		bocal_init(t_scene *scene)
{
	bocal_init_objects1(scene);
	bocal_init_objects2(scene);
}
