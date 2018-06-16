/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   matrix_transform.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 19:56:58 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/16 19:57:20 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	set_cam_translate_matrix(t_camera *cam)
{
	cam->translate_matrix = mat_translate(cam->location);
}

void	set_cam_rotate_matrix(t_camera *cam)
{
	mat4	rotation_matrix;

	rotation_matrix = mat_rotatex(cam->rotation.x);
	rotation_matrix = mat_mult_mat(mat_rotatey(cam->rotation.y),
		rotation_matrix);
	cam->rotate_matrix = mat_mult_mat(mat_rotatez(cam->rotation.z),
		rotation_matrix);
}

void	obj_transform_mats(t_object *obj)
{
	mat4 translate;
	mat4 rotate;
	mat4 scale;

	translate = mat_translate(obj->location);
	rotate = mat_rotatex(obj->rotation.x);
	rotate = mat_mult_mat(mat_rotatey(obj->rotation.y), rotate);
	rotate = mat_mult_mat(mat_rotatez(obj->rotation.z), rotate);
	scale = mat_scale(obj->scale);
	obj->from_local = mat_mult_mat(mat_mult_mat(translate, rotate),
		scale);
	obj->to_local = mat_invert(obj->from_local);
}
