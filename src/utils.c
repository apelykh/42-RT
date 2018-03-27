#include <stdio.h>
#include "rt.h"

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
