#include <stdio.h>
#include "rt.h"

void	ft_putstr(char const *s)
{
	if (s == NULL)
		return ;
	while (*s != '\0')
	{
		write(1, s, 1);
		s++;
	}
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


static float	clamp(float x)
{
	return x < 0.0f ? 0.0f : x > 1.0f ? 1.0f : x;
}

int		to_uchar(float x)
{
	return (unsigned char)(clamp(x) * 255 + .5);
}

static int		to_int(float x)
{
	return (int)(clamp(x) * 255 + .5);
}

void	save_image(cl_float3 *pixels)
{
	FILE *f = fopen("./saved_images/9_spheres.ppm", "w");
	fprintf(f, "P3\n%d %d\n%d\n", WIN_WIDTH, WIN_HEIGHT, 255);

	for (int i = 0; i < WIN_WIDTH * WIN_HEIGHT; i++)
		fprintf(f, "%d %d %d ",
		to_int(pixels[i].s[0]),
		to_int(pixels[i].s[1]),
		to_int(pixels[i].s[2]));

	printf("[+] Image saved!\n");
}
