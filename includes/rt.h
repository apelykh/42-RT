/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:30:16 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/15 23:06:32 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_H
# define RT_H

#include <unistd.h>
#include <fcntl.h>

# define CL_USE_DEPRECATED_OPENCL_1_2_APIS

// # include <SDL2/SDL.h>
# include "cJSON.h"
# include "/Users/apelykh/.brew/Cellar/sdl2/2.0.8/include/SDL2/SDL.h"

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

# define AMBIENT		0
# define POINT			1
# define PARALLEL		2
# define SPOT			3

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

typedef cl_float16		mat4;

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
	cl_float			transparency;
	cl_float			spec_exp;
	cl_float			ior;
	cl_float			kr;
	mat4 				from_local;
	mat4 				to_local;
}						t_object;

typedef struct			s_light
{
	cl_int				type;
	cl_float			angle;
	cl_float3			dir;
	cl_float3			location;
	cl_float3			emission;
}						t_light;

typedef struct			s_camera
{
	cl_float3			location;
	cl_float3			rotation;
	cl_float			fov;
	mat4				translate_matrix;
	mat4				rotate_matrix;
}						t_camera;

typedef struct			s_control_flags
{
	int					move_z;
	int					move_x;
	int					move_y;
	int					rotate_y;
	int					rotate_x;
	int					quit;
}						t_control_flags;

typedef struct			s_scene
{
	t_camera			*cam;
	t_object 			*objects;
	cl_int				num_objects;
	t_light				*lights;
	cl_int				num_lights;
	cl_int				im_width;
	cl_int				im_height;
	t_control_flags		controls;
	cl_int				sepia;
}						t_scene;

cl_int2		init_int2(cl_int x, cl_int y);
cl_float3	init_vec3(cl_float x, cl_float y, cl_float z);
cl_float3	add_vec3(cl_float3 a, cl_float3 b);
cl_float3	init_norm_vec3(cl_float x, cl_float y, cl_float z);
cl_float4	init_vec4(cl_float x, cl_float y, cl_float z, cl_float w);

cJSON		*cj_obj(cJSON *root, char *item_name);
void		cj_get_bool(cl_bool *target, cJSON *object, char *item_name);
char		*cj_get_str(cJSON *object, char *item_name);
cl_int		cj_get_obj_type(char *string_type);
cl_int		cj_get_light_type(char *string_type);
int			is_complex_obj(cl_int object_type);


/*
 *	--------------- PARSING ---------------
 */
void		open_scene(char *scene_path, t_scene *scene);
void		parse_camera(cJSON *cj_root, t_scene *scene);
void		parse_lights(cJSON *cj_root, t_scene *scene);
void		parse_objects(cJSON *cj_root, t_scene *scene);
void        bocal_init(t_scene *scene);
void	    save_float(cl_float *target, cJSON *float_obj, float min, float max);
void	    save_float3(cl_float3 *target, cJSON *cj_float3_arr, float min, float max);

/*
 *	--------------- values_init.c ---------------
 */
void		controls_init_empty(t_control_flags *controls);
void        camera_init_empty(t_camera *camera);
void        light_init_empty(t_light *light);
void        object_init_empty(t_object *obj);

/*
 *	--------------- controls.c ---------------
 */
// void		handle_keys(t_scene *scene, SDL_Event *event);
void		move_z(t_scene *scene, int dir);
void		move_x(t_scene *scene, int dir);
void		move_y(t_scene *scene, int dir);
void		rotate_y(t_scene *scene, int dir);
void		rotate_x(t_scene *scene, int dir);

void		key_press(t_scene *scene, SDL_Event event);
void		key_release(t_scene *scene, SDL_Event event);

/*
 *	--------------- sdl_utils.c ---------------
 */
void	sdl_init(t_sdl_context *sdl_context);
void	sdl_render(t_sdl_context *sdl_context, t_scene *scene);
void	sdl_cleanup(t_sdl_context *sdl_context);

void	init_cl(t_cl_context *cl_context);
void	alloc_cl_buffers(t_cl_context *cl_context, t_scene *scene);
void	set_kernel_args(t_cl_context *cl_context, t_scene *scene);
void	cl_cleanup(t_cl_context *cl_context);

// float	clamp_float_minmax(float x, float min, float max);
// cl_float3	clamp_float3_minmax(cl_float3 x, float min, float max);

float		minmax_float(float x, float min, float max);
cl_float3	minmax_float3(cl_float3 x, float min, float max);

void		ft_error(char *text1, char *text2);
void        ft_putstr(char const *s);

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

void		print_camera(t_scene *scene);
void		print_lights(t_scene *scene);
void		print_objects(t_scene *scene);

#endif
