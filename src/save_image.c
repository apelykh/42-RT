/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   save_image.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/16 17:33:42 by efedoryc          #+#    #+#             */
/*   Updated: 2018/06/17 17:35:25 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "rt.h"

static unsigned char	*create_bitmap_file_header(int height, int width)
{
	int				file_size;
	unsigned char	*file_header;

	file_header = (unsigned char *)malloc(sizeof(unsigned char) * 14);
	array_nuller(&file_header, 14);
	file_size = FILE_HEADER_SIZE + INFO_HEADER_SIZE
				+ BYTES_PER_PIXEL * height * width;
	file_header[0] = (unsigned char)('B');
	file_header[1] = (unsigned char)('M');
	file_header[2] = (unsigned char)(file_size);
	file_header[3] = (unsigned char)(file_size >> 8);
	file_header[4] = (unsigned char)(file_size >> 16);
	file_header[5] = (unsigned char)(file_size >> 24);
	file_header[10] = (unsigned char)(FILE_HEADER_SIZE + INFO_HEADER_SIZE);
	return (file_header);
}

static unsigned char	*create_bitmap_info_header(int height, int width)
{
	unsigned char *info_header;

	info_header = (unsigned char *)malloc(sizeof(unsigned char) * 40);
	array_nuller(&info_header, 40);
	info_header[0] = (unsigned char)(INFO_HEADER_SIZE);
	info_header[4] = (unsigned char)(width);
	info_header[5] = (unsigned char)(width >> 8);
	info_header[6] = (unsigned char)(width >> 16);
	info_header[7] = (unsigned char)(width >> 24);
	info_header[8] = (unsigned char)(height);
	info_header[9] = (unsigned char)(height >> 8);
	info_header[10] = (unsigned char)(height >> 16);
	info_header[11] = (unsigned char)(height >> 24);
	info_header[12] = (unsigned char)(1);
	info_header[14] = (unsigned char)(BYTES_PER_PIXEL * 8);
	return (info_header);
}

static void				generate_bitmap_image(unsigned char *image,
						int height, int width, char *image_file_name)
{
	unsigned char	*file_header;
	unsigned char	*info_header;
	unsigned char	*padding;
	int				i;
	FILE			*image_file;

	file_header = create_bitmap_file_header(height, width);
	info_header = create_bitmap_info_header(height, width);
	padding = (unsigned char *)malloc(sizeof(unsigned char) * 3);
	array_nuller(&padding, 3);
	image_file = fopen(image_file_name, "wb");
	fwrite(file_header, 1, FILE_HEADER_SIZE, image_file);
	fwrite(info_header, 1, INFO_HEADER_SIZE, image_file);
	i = height - 1;
	while (i >= 0)
	{
		fwrite(image + (i-- * width * BYTES_PER_PIXEL),
					BYTES_PER_PIXEL, width, image_file);
		fwrite(padding, 1, PADDING_SIZE, image_file);
	}
	array_free(&file_header, 14);
	array_free(&info_header, 40);
	array_free(&padding, 3);
	fclose(image_file);
}

void					save_image(cl_uchar4 *pixels)
{
	unsigned char	image[WIN_HEIGHT][WIN_WIDTH][BYTES_PER_PIXEL];
	int				i;
	int				j;

	i = 0;
	while (i < WIN_HEIGHT)
	{
		j = 0;
		while (j < WIN_WIDTH)
		{
			image[i][j][0] = (unsigned char)(pixels[j + i * WIN_WIDTH].x);
			image[i][j][1] = (unsigned char)(pixels[j + i * WIN_WIDTH].y);
			image[i][j][2] = (unsigned char)(pixels[j + i * WIN_WIDTH].z);
			j++;
		}
		i++;
	}
	generate_bitmap_image((unsigned char *)image,
				WIN_HEIGHT, WIN_WIDTH, "screenshot.bmp");
}
