#include "rt.h"

void	init_sdl(t_sdl_context *sdl_context)
{
	Uint32 render_flags;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
	{
		printf("SDL Init error: %s\n", SDL_GetError());
		exit(-1);
	}

	sdl_context->win = SDL_CreateWindow("RT", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
	if (!sdl_context->win)
	{
		printf("[-] Error creating window: %s\n", SDL_GetError());
		sdl_cleanup(sdl_context);
		exit(-1);
	}

	render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	sdl_context->rend = SDL_CreateRenderer(sdl_context->win, -1, render_flags);
	if (!sdl_context->rend)
	{
		printf("[-] Error creating renderer: %s\n", SDL_GetError());
		sdl_cleanup(sdl_context);
		exit(-1);
	}
	sdl_context->pixels = (cl_uchar4*)malloc(WIN_WIDTH * WIN_HEIGHT * sizeof(cl_uchar4));
}

void	sdl_cleanup(t_sdl_context *sdl_context)
{
	if (sdl_context->win)
		SDL_DestroyWindow(sdl_context->win);

	if (sdl_context->rend)
		SDL_DestroyRenderer(sdl_context->rend);

	free(sdl_context->pixels);
	sdl_context->pixels = NULL;
	SDL_Quit();
}
