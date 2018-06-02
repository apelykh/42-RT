#include "rt.h"

static void key_press_rotation(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
		scene->controls.isrotating = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT)
		scene->controls.isrotating = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
		scene->controls.ispitching = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
		scene->controls.ispitching = -1;
}

static void key_press_movement(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_W)
		scene->controls.ismoving = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_S)
		scene->controls.ismoving = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_D)
		scene->controls.isstrafing = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_A)
		scene->controls.isstrafing = -1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
		scene->controls.isflying = 1;
	else if (event.key.keysym.scancode == SDL_SCANCODE_E)
		scene->controls.isflying = -1;
	else
		key_press_rotation(scene, event);
}

void key_press(t_scene *scene, SDL_Event event)
{
	if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		scene->controls.quit = 1;
	else
		key_press_movement(scene, event);
}