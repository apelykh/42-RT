/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:10:57 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 17:31:07 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

void		ft_putstr(char const *s)
{
	if (s == NULL)
		return ;
	while (*s != '\0')
	{
		write(1, s, 1);
		s++;
	}
}

void		ft_error(char *text1, const char *text2)
{
	ft_putstr(text1);
	ft_putstr("\n");
	if (text2 != NULL)
	{
		ft_putstr(text2);
		ft_putstr("\n");
	}
	exit(EXIT_FAILURE);
}

void		array_nuller(unsigned char **array, int size)
{
	int i;

	i = 0;
	while (i < size)
		(*array)[i++] = 0;
}

void		array_free(unsigned char **array, int size)
{
	int i;

	i = 0;
	while (i < size)
		(*array)[i++] = 0;
	free((void *)*array);
}
