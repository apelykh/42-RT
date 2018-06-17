/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rt.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: apelykh <apelykh@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/15 22:30:16 by apelykh           #+#    #+#             */
/*   Updated: 2018/06/17 17:31:21 by apelykh          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RT_H
# define RT_H

# include <unistd.h>
# include <fcntl.h>

# define CL_USE_DEPRECATED_OPENCL_1_2_APIS

# include "SDL.h"
# include "cJSON.h"

# ifdef __APPLE__
#  include <OpenCL/opencl.h>
# else
#  include <CL/cl.h>
# endif

# define WIN_WIDTH			1280
# define WIN_HEIGHT			768

# define MAX_SRC_SIZE		(0x100000)

# define PLANE				0
# define SPHERE				1
# define CYLINDER			2
# define CONE				3
# define BOX				4
# define UNION				5
# define INTERSECTION		6
# define DIFFERENCE			7
# define CLIPPING			8

# define AMBIENT			0
# define POINT				1
# define PARALLEL			2
# define SPOT				3

# define BYTES_PER_PIXEL	3
# define FILE_HEADER_SIZE	14
# define INFO_HEADER_SIZE	40
# define PADDING_SIZE		(4 - (WIN_WIDTH * BYTES_PER_PIXEL) % 4) % 4

typedef struct				s_sdl_context
{
	SDL_Window				*win;
	SDL_Renderer			*rend;
	SDL_Surface				*surf;
	SDL_Texture				*tex;
	cl_uchar4				*pixels;
}							t_sdl_context;

typedef struct				s_cl_context
{
	cl_context				context;
	cl_program				program;
	cl_command_queue		command_queue;
	char					*kernel_src;
	size_t					src_size;
	cl_kernel				kernel;
	cl_mem					pixels_buf;
	cl_mem					objects_buf;
	cl_mem					lights_buf;
	cl_mem					cam_buf;
}							t_cl_context;

typedef cl_float16			t_mat4;

typedef struct				s_object
{
	cl_int					id;
	cl_bool					hidden;
	cl_int					type;
	cl_int2					operand;
	cl_bool					capped;
	cl_float3				location;
	cl_float3				rotation;
	cl_float3				scale;
	cl_float3				color;
	cl_float				transparency;
	cl_float				spec_pow;
	cl_float				ior;
	cl_float				kr;
	t_mat4					from_local;
	t_mat4					to_local;
}							t_object;

typedef struct				s_light
{
	cl_int					type;
	cl_float				angle;
	cl_float3				dir;
	cl_float3				location;
	cl_float3				emission;
}							t_light;

typedef struct				s_camera
{
	cl_float3				location;
	cl_float3				rotation;
	cl_float				fov;
	t_mat4					translate_matrix;
	t_mat4					rotate_matrix;
}							t_camera;

typedef struct				s_control_flags
{
	int						move_z;
	int						move_x;
	int						move_y;
	int						rotate_y;
	int						rotate_x;
	int						quit;
}							t_control_flags;

typedef struct				s_scene
{
	t_camera				*cam;
	t_object				*objects;
	cl_int					num_objects;
	t_light					*lights;
	cl_int					num_lights;
	cl_int					im_width;
	cl_int					im_height;
	t_control_flags			controls;
	cl_int					sepia;
	cl_int					to_save_image;
}							t_scene;

/*
**	--------------- parsing ---------------
*/
void						controls_init_empty(t_control_flags *controls);
void						camera_init_empty(t_camera *camera);
void						light_init_empty(t_light *light);
void						object_init_empty(t_object *obj);
void						open_scene(char *scene_path, t_scene *scene);
void						parse_camera(cJSON *cj_root, t_scene *scene);
void						parse_lights(cJSON *cj_root, t_scene *scene);
void						parse_objects(cJSON *cj_root, t_scene *scene);
void						bocal_init(t_scene *scene);
void						save_float(cl_float *target, cJSON *float_obj,
								float min, float max);
void						save_float3(cl_float3 *target, cJSON *cj_float3_arr,
								float min, float max);
cJSON						*cj_obj(cJSON *root, char *item_name);
void						cj_get_bool(cl_bool *target, cJSON *object,
								char *item_name);
char						*cj_get_str(cJSON *object, char *item_name);
cl_int						cj_get_obj_type(char *string_type);
cl_int						cj_get_light_type(char *string_type);

/*
**	--------------- vector operations ---------------
*/
cl_int2						init_int2(cl_int x, cl_int y);
cl_float3					init_vec3(cl_float x, cl_float y, cl_float z);
cl_float3					add_vec3(cl_float3 a, cl_float3 b);
cl_float3					init_norm_vec3(cl_float x, cl_float y, cl_float z);
cl_float4					init_vec4(cl_float x, cl_float y, cl_float z,
								cl_float w);
cl_float3					vec_norm(cl_float3 a);

/*
**	--------------- controls ---------------
*/
void						move_z(t_scene *scene, int dir);
void						move_x(t_scene *scene, int dir);
void						move_y(t_scene *scene, int dir);
void						rotate_y(t_scene *scene, int dir);
void						rotate_x(t_scene *scene, int dir);
void						key_press(t_scene *scene, SDL_Event event);
void						key_release(t_scene *scene, SDL_Event event);

/*
**	--------------- SDL utils ---------------
*/
void						sdl_init(t_sdl_context *sdl_context);
void						sdl_render(t_sdl_context *sdl_context,
								t_scene *scene);
void						sdl_cleanup(t_sdl_context *sdl_context);

/*
**	--------------- OpenCL utils ---------------
*/
void						cl_init(t_cl_context *context);
void						cl_alloc_buffers(t_cl_context *context,
								t_scene *scene);
void						cl_set_kernel_args(t_cl_context *context,
								t_scene *scene);
void						cl_update_pixels(t_scene *scene, t_sdl_context
								*sdl_context, t_cl_context *context);
void						cl_cleanup(t_cl_context *context);

/*
**	--------------- matrix operations ---------------
*/
t_mat4						mat_translate(cl_float3 v);
t_mat4						mat_scale(cl_float3 s);
t_mat4						mat_rotatex(cl_float angle);
t_mat4						mat_rotatey(cl_float angle);
t_mat4						mat_rotatez(cl_float angle);
t_mat4						mat_invert(t_mat4 m);
t_mat4						mat_transpose(t_mat4 m);
t_mat4						mat_mult_mat(t_mat4 a, t_mat4 b);
cl_float4					mat_mult_vec(t_mat4 m, cl_float4 v);
void						obj_transform_mats(t_object *obj);
void						set_cam_translate_matrix(t_camera *cam);
void						set_cam_rotate_matrix(t_camera *cam);

/*
**	--------------- utils.c ---------------
*/
void						ft_error(char *text1, const char *text2);
void						ft_putstr(char const *s);
void						array_nuller(unsigned char **array, int size);
void						array_free(unsigned char **array, int size);
void						save_image(cl_uchar4 *pixels);

#endif
