/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls_press.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 21:04:18 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/15 22:33:19 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static void	key_press_rotation(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		scene->controls.rotate_y = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
		scene->controls.rotate_y = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
		scene->controls.rotate_x = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
		scene->controls.rotate_x = -1;
}

static void	key_press_movement(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_W)
		scene->controls.move_z = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_S)
		scene->controls.move_z = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_D)
		scene->controls.move_x = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_A)
		scene->controls.move_x = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
		scene->controls.move_y = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_E)
		scene->controls.move_y = 1;
	else
		key_press_rotation(scene, event);
}

static void	toggle_sepia(t_scene *scene)
{
	if (scene->sepia == 1)
		scene->sepia = 0;
	else
		scene->sepia = 1;
}

void		key_press(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		scene->controls.quit = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_C)
		toggle_sepia(scene);
	else
		key_press_movement(scene, event);
}
