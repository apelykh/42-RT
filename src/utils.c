/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:10:57 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/15 23:08:45 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
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

void		ft_error(char *text1, char *text2)
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

float		minmax_float(float x, float min, float max)
{
	if (x < min || x > max)
	{
		//!!!!!!!!!!
		printf("[-] Float value outside the limit\n %f\n", x);
		exit(EXIT_FAILURE);
	}
	return (x);
}

cl_float3	minmax_float3(cl_float3 x, float min, float max)
{
	cl_float3	res;
	cl_int		error;

	error = 0;
	(x.x < min || x.x > max) && (error = 1);
	(x.y < min || x.y > max) && (error = 1);
	(x.z < min || x.z > max) && (error = 1);
	if (error)
	{
		//!!!!!!!!!!
		printf("[-] Float3 value outside the limit\n");
		exit(EXIT_FAILURE);
	}
	res.x = minmax_float(x.x, min, max);
	res.y = minmax_float(x.y, min, max);
	res.z = minmax_float(x.z, min, max);
	return (res);
}
