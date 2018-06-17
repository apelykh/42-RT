/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sdl_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:19:29 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 17:32:41 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void	sdl_init(t_sdl_context *sdl_context)
{
	Uint32 render_flags;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		ft_error("[-] SDL Init error:", SDL_GetError());
	sdl_context->win = SDL_CreateWindow("RT", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, 0);
	if (!sdl_context->win)
	{
		sdl_cleanup(sdl_context);
		ft_error("[-] Error creating window:", SDL_GetError());
	}
	render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	sdl_context->rend = SDL_CreateRenderer(sdl_context->win, -1, render_flags);
	if (!sdl_context->rend)
	{
		sdl_cleanup(sdl_context);
		ft_error("[-] Error creating renderer:", SDL_GetError());
	}
	sdl_context->pixels = (cl_uchar4*)malloc(
		WIN_WIDTH * WIN_HEIGHT * sizeof(cl_uchar4));
}

void	sdl_render(t_sdl_context *sdl_context, t_scene *scene)
{
	sdl_context->surf = SDL_CreateRGBSurfaceFrom(sdl_context->pixels,
		scene->im_width, scene->im_height, 32,
		scene->im_width * 4, 0, 0, 0, 0);
	sdl_context->tex = SDL_CreateTextureFromSurface(sdl_context->rend,
		sdl_context->surf);
	SDL_RenderClear(sdl_context->rend);
	SDL_RenderCopy(sdl_context->rend, sdl_context->tex, NULL, NULL);
	SDL_RenderPresent(sdl_context->rend);
	SDL_FreeSurface(sdl_context->surf);
	SDL_DestroyTexture(sdl_context->tex);
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
