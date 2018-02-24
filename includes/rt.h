#ifndef RT_H
# define RT_H

# define CL_USE_DEPRECATED_OPENCL_1_2_APIS

# ifdef __APPLE__
# include <OpenCL/opencl.h>
# else
# include <CL/cl.h>
# endif

# define WIN_WIDTH 1280
# define WIN_HEIGHT 720

// typedef struct			s_sdl_context
// {
// 	SDL_Window 			*win;
// 	SDL_Renderer		*rend;
// 	SDL_Surface			*surf;
// 	SDL_Texture			*tex;
// 	unsigned char		*data;
// }						t_sdl_context;

typedef struct			s_cl_context
{
	cl_context			context;
	cl_program			program;
	cl_command_queue	command_queue;
	char				*kernel_src;
	size_t				src_size;
	cl_kernel			kernel;
}						t_cl_context;

// dummy variables are required for memory alignment
// float3 is considered as float4 by OpenCL
typedef struct			s_sphere
{
	cl_float			radius;
	cl_float			dummy1;   
	cl_float			dummy2;
	cl_float			dummy3;
	cl_float3			pos;
	cl_float3			color;
	cl_float3			emi;
}						t_sphere;

#endif