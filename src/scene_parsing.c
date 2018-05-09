#include <stdio.h>
#include <string.h>
#include "cJSON.h"
#include "rt.h"
// #include "rtcjp.h"

// cl_float3 cjcjGetFloat3(cJSON *root, char *item_name)
// {
// 	cl_float3 result;
// 	cJSON *cj_float3array;

// 	cj_float3array = cJSON_GetObjectItem(root, item_name);
// 	result.x = cJSON_GetArrayItem(cj_float3array, 0)->valuedouble;
// 	result.y = cJSON_GetArrayItem(cj_float3array, 1)->valuedouble;
// 	result.z = cJSON_GetArrayItem(cj_float3array, 2)->valuedouble;
// 	return result;
// }

// cl_float cjGetFloat(cJSON *object, char *item_name)
// {
// 	return cJSON_GetObjectItem(object, item_name)->valuedouble;
// }

// cl_int cjGetInt(cJSON *object, char *item_name)
// {
// 	return cJSON_GetObjectItem(object, item_name)->valueint;
// }

// char *cjGetString(cJSON *object, char *item_name)
// {
// 	return cJSON_GetObjectItem(object, item_name)->valuestring;
// }

// int cjGetType(char *stringType)
// {
// 	char result;

// 	if (!strcmp(stringType, "CYLINDER"))
// 		result = 1;
// 	else if (!strcmp(stringType, "DIFFERENCE"))
// 		result = 2;
// 	else
// 		result = 0;

// 	return result;
// }

// t_camera cameraInit(cJSON *cj_root)
// {
// 	t_camera	camera;
// 	cJSON		*cj_camera;

// 	cj_camera = cJSON_GetObjectItem(cj_root, "camera");
// 	camera.loc = cjcjGetFloat3(cj_camera, "location");
// 	camera.rot = cjcjGetFloat3(cj_camera, "rotation");
// 	camera.fov = cjGetFloat(cj_camera, "fov");

// 	return camera;
// }
// t_light *lightsInit(cJSON *cj_root)
// {
// 	t_light	*lights;
// 	cJSON	*cj_lights;
// 	int		lights_count;
// 	cJSON	*current_object;
// 	int		i;

// 	cj_lights = cJSON_GetObjectItem(cj_root, "lights");
// 	lights_count = cJSON_GetArraySize(cj_lights);
// 	lights = (t_light *)malloc(sizeof(t_light) * lights_count);

// 	i = 0;
// 	while (i < lights_count)
// 	{
// 		current_object = cJSON_GetArrayItem(cj_lights, i);
// 		lights[i].loc = cjcjGetFloat3(current_object, "location");
// 		lights[i].emi = cjcjGetFloat3(current_object, "emission");
// 		i++;
// 	}

// 	return lights;
// }

// t_object *objectsInit(cJSON *cj_root)
// {
// 	t_object	*objects;
// 	cJSON		*cj_objects;
// 	cJSON		*current_object;
// 	int			objects_count;
// 	int			i;

// 	cj_objects = cJSON_GetObjectItem(cj_root, "objects");
// 	objects_count = cJSON_GetArraySize(cj_objects);
// 	objects = (t_object *)malloc(sizeof(t_object) * objects_count);

// 	i = 0;
// 	while (i < objects_count)
// 	{
// 		current_object = cJSON_GetArrayItem(cj_objects, i);
// 		objects[i].inherits_from = cjGetInt(current_object, "inherits_from");
// 		objects[i].type = cjGetType(cjGetString(current_object, "type"));
// 		objects[i].capped = cjGetInt(current_object, "capped");
// 		objects[i].loc = cjcjGetFloat3(current_object, "location");
// 		objects[i].rot = cjcjGetFloat3(current_object, "rotation");
// 		objects[i].sca = cjcjGetFloat3(current_object, "scale");
// 		objects[i].color = cjcjGetFloat3(current_object, "color");
// 		objects[i].emi = cjcjGetFloat3(current_object, "emission");
// 		objects[i].diffuse = cjGetFloat(current_object, "diffuse");
// 		objects[i].specular = cjGetFloat(current_object, "specular");
// 		objects[i].spec_exp = cjGetFloat(current_object, "specular_exp");
// 		i++;
// 	}

// 	return objects;
// }

// void sceneInit(char *strJSON, t_camera *camera, t_light **lights, t_object **objects)
// {
// 	cJSON *cj_root;

// 	cj_root = cJSON_Parse(strJSON);
// 	if (!cj_root)
// 	{
// 		printf("Error: %s\n", cJSON_GetErrorPtr());
// 		exit(0);
// 	}

// 	*camera = cameraInit(cj_root);
// 	*lights = lightsInit(cj_root);
// 	*objects = objectsInit(cj_root);

// 	cJSON_Delete(cj_root);
// }

void	parse_scene(char *scene_path)
{
	
    // char *strJSON = "{\"camera\":{\"location\":[0.0,1.0,10.0],\"rotation\":[0,0,0],\"field_of_view\":45.0},\"lights\":[{\"location\":[0.0,2.75,2.75],\"emission\":[1.0,1.0,1.0]},{\"location\":[3.0,5.0,-7.0],\"emission\":[0.5,0.5,0.5]}],\"objects\":[{\"inherits_from\":-1,\"type\":\"PLANE\",\"capped\":true,\"scale\":[1.0,1.0,1.0],\"rotation\":[0.0,0.0,0.0],\"location\":[0.0,-1.0,0.0],\"color\":[0.9,0.8,0.7],\"emission\":[0.0,0.0,0.0],\"diffuse\":1.0,\"specular\":0.0,\"specular_exp\":0.5},{\"inherits_from\":-1,\"type\":\"CYLINDER\",\"scale\":[1.0,1.0,1.0],\"rotation\":[-45.0,0.0,15.0],\"location\":[0.0,0.5,-2.1],\"color\":[0.3,0.3,0.3],\"emission\":[0.0,0.0,0.0],\"capped\":true,\"diffuse\":0.7,\"specular\":0.9,\"specular_exp\":20.0}]}";

	char *scene;

	FILE *fp;

	fp = fopen(scene_path, "r");
	if (!fp)
	{
		fprintf(stderr, "[-] Failed to load scene file.\n");
		exit(1);
	}
	scene = (char*)malloc(MAX_SOURCE_SIZE);
	fread(scene, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	printf("%s\n", scene);

	// t_camera camera;
	// t_light *lights;
	// t_object *objects;

	// sceneInit(strJSON, &camera, &lights, &objects);

	// printf("%f\n", camera.fov);
	// printf("Camera: %f, %f, %f\n", camera.rot.x, camera.rot.y, camera.rot.z);
	// printf("Light %f\n", lights[0].loc.y);
	// printf("Object %f\n", objects[0].loc.y);

	// TODO: Save lights and objects count

	return (0);
}