#ifndef RT_H
# define RT_H

# define CL_USE_DEPRECATED_OPENCL_1_2_APIS

# include <SDL2/SDL.h>
// # include "/Users/mdubrovs/.brew/Cellar/sdl2/2.0.8/include/SDL2/SDL.h"
// # include <SDL2/SDL_timer.h>
// # include <SDL2/SDL_image.h>

# ifdef __APPLE__
# include <OpenCL/opencl.h>
# else
# include <CL/cl.h>
# endif

# define WIN_WIDTH 1280
# define WIN_HEIGHT 768

# define MAX_SOURCE_SIZE (0x100000)

# define PLANE			0
# define SPHERE			1
# define CYLINDER		2
# define CONE			3
# define BOX			4
# define UNION			5
# define INTERSECTION	6
# define DIFFERENCE		7
# define CLIPPING		8

typedef struct			s_sdl_context
{
	SDL_Window 			*win;
	SDL_Renderer		*rend;
	SDL_Surface			*surf;
	SDL_Texture			*tex;
	cl_uchar4			*pixels;
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

typedef cl_float16 mat4;

// dummy variables are required for memory alignment
// float3 is considered as float4 by OpenCL
typedef struct			s_object
{
	cl_int				id;
	cl_bool				hidden;
	cl_int				type;
	cl_int2				operand;
	cl_bool				capped;
	cl_float3			location;
	cl_float3			rotation;
	cl_float3			scale;
	cl_float3			color;
	cl_float3			emi;
	cl_float			diffuse;
	cl_float			specular;
	cl_float			spec_exp;
	mat4 				from_local;
	mat4 				to_local;
	// cl_float			dummy1;
	// cl_float			dummy2;
}						t_object;

typedef struct			s_light
{
	cl_float3			location;
	cl_float3			emi;
}						t_light;

typedef struct			s_camera
{
	cl_float3			location;
	cl_float3			rotation;
	cl_float			fov;
	mat4				translate_matrix;
	mat4				rotate_matrix;
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
cl_float3	add_vec3(cl_float3 a, cl_float3 b);
cl_float3	init_norm_vec3(cl_float x, cl_float y, cl_float z);
cl_float4	init_vec4(cl_float x, cl_float y, cl_float z, cl_float w);

void	init_scene1(t_scene *scene);

void	init_sdl(t_sdl_context *sdl_context);
void	sdl_cleanup(t_sdl_context *sdl_context);

void	init_cl(t_cl_context *cl_context);
void	alloc_cl_buffers(t_cl_context *cl_context, t_scene *scene);
void	set_kernel_args(t_cl_context *cl_context, t_scene *scene);
void	cl_cleanup(t_cl_context *cl_context);

void	save_image(cl_float3 *pixels);
int		to_uchar(float x);

//matrix funcs
void		obj_transform_mats(t_object *obj);
mat4		mat_translate(cl_float3 v);
mat4		mat_scale(cl_float3 s);
mat4		mat_rotatex(cl_float angle);
mat4		mat_rotatey(cl_float angle);
mat4		mat_rotatez(cl_float angle);
mat4		mat_invert(mat4 m);
mat4		mat_invert_(mat4 m, cl_float det);
mat4		mat_transpose(mat4 m);
mat4		mat_mult_mat(mat4 a, mat4 b);
cl_float4	mat_mult_vec(mat4 m, cl_float4 v);
void		set_cam_translate_matrix(t_camera *cam);
void		set_cam_rotate_matrix(t_camera *cam);

#endif
