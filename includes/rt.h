#ifndef RT_H
# define RT_H

# define CL_USE_DEPRECATED_OPENCL_1_2_APIS

# include <SDL2/SDL.h>
# include <SDL2/SDL_timer.h>
# include <SDL2/SDL_image.h>

# ifdef __APPLE__
# include <OpenCL/opencl.h>
# else
# include <CL/cl.h>
# endif

# define WIN_WIDTH 1600
# define WIN_HEIGHT 900

# define MAX_SOURCE_SIZE (0x100000)

# define PLANE		0
# define SPHERE		1
# define CYLINDER	2
# define CONE		3

typedef struct			s_sdl_context
{
	SDL_Window 			*win;
	SDL_Renderer		*rend;
	SDL_Surface			*surf;
	SDL_Texture			*tex;
	unsigned char		*data;
}						t_sdl_context;

typedef struct			s_cl_context
{
	cl_context			context;
	cl_program			program;
	cl_command_queue	command_queue;
	char				*kernel_src;
	size_t				src_size;
	cl_kernel			kernel;
	cl_mem				pixels_buf;
	cl_mem				objects_buf;
	cl_mem				lights_buf;
	cl_mem				cam_buf;
}						t_cl_context;

// dummy variables are required for memory alignment
// float3 is considered as float4 by OpenCL
typedef struct			s_object
{
	cl_int				type;
	cl_float			radius;
	cl_float			radius2;
	cl_float			k;
	cl_float3			pos;
	cl_float3			dir;
	cl_float3			color;
	cl_float3			emi;
	cl_float			diffuse;
	cl_float			specular;
	cl_float			spec_exp;
	// cl_float			dummy1;
	// cl_float			dummy2;
}						t_object;

typedef struct			s_light
{
	cl_float3			pos;
	cl_float3			emi;
}						t_light;

typedef struct			s_camera
{
	cl_float3			pos;
}						t_camera;

typedef struct			s_scene
{
	t_camera			*cam;
	t_object 			*objects;
	cl_int				num_objects;
	t_light				*lights;
	cl_int				num_lights;
	cl_int				im_width;
	cl_int				im_height;
}						t_scene;

cl_float3	init_vec3(cl_float x, cl_float y, cl_float z);
cl_float3	init_norm_vec3(cl_float x, cl_float y, cl_float z);

void	init_scene1(t_scene *scene);
void	init_scene2(t_scene *scene);

void	init_sdl(t_sdl_context *sdl_context);
void	sdl_cleanup(t_sdl_context *sdl_context);

void	init_cl(t_cl_context *cl_context);
void	alloc_cl_buffers(t_cl_context *cl_context, t_scene *scene);
void	set_kernel_args(t_cl_context *cl_context, t_scene *scene);
void	cl_cleanup(t_cl_context *cl_context);

void	save_image(cl_float3 *pixels);
int		to_uchar(float x);

#endif