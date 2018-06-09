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

void	ft_error(char *text1, char *text2)
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

cl_float3	clamp_float3_minmax(cl_float3 x, float min, float max)
{
	cl_float3 res;

	res.x = clamp_float_minmax(x.x, min, max);
	res.y = clamp_float_minmax(x.y, min, max);
	res.z = clamp_float_minmax(x.z, min, max);
	return (res);
}

cl_float3	minmax_float3(cl_float3 x, float min, float max)
{
	cl_float3   res;
    cl_int      error;

    error = 0;
    (x.x < min || x.x > max) && (error = 1);
    (x.y < min || x.y > max) && (error = 1);
    (x.z < min || x.z > max) && (error = 1);

    if (error)
    {
        printf("[-] Float3 value outside the limit\n");
        exit(EXIT_FAILURE);
    }

	res.x = minmax_float(x.x, min, max);
	res.y = minmax_float(x.y, min, max);
	res.z = minmax_float(x.z, min, max);
	return (res);
}

float	minmax_float(float x, float min, float max)
{
	if (x < min || x > max)
    {
        printf("[-] Float value outside the limit\n %f\n", x);
        exit(EXIT_FAILURE);
    }
    return (x);
}

float	clamp_float_minmax(float x, float min, float max)
{
	float res;

	res = (x > max) ? max : x;
	if (x < min)
		res = min;
	return (res);
}

static float	clamp(float x)
{
	return x < 0.0f ? 0.0f : x > 1.0f ? 1.0f : x;
}

int		to_uchar(float x)
{
	return (unsigned char)(clamp(x) * 255 + .5);
}