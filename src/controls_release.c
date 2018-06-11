#include "rt.h"

static void	key_release_rotation(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		scene->controls.rotate_y = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
		scene->controls.rotate_y = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
		scene->controls.rotate_x = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
		scene->controls.rotate_x = 0;
}

static void	key_release_movement(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_W)
		scene->controls.move_z = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_S)
		scene->controls.move_z = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_D)
		scene->controls.move_x = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_A)
		scene->controls.move_x = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
		scene->controls.move_y = 0;
	else if (event.key.keysym.scancode == SDL_SCANCODE_E)
		scene->controls.move_y = 0;
	else
		key_release_rotation(scene, event);
}

void		key_release(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		scene->controls.quit = 1;
	else
		key_release_movement(scene, event);
}
